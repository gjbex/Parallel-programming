#!/usr/bin/env python3
'''Benchmark the serial heat solver over grid and tile sizes.'''

from __future__ import annotations

import argparse
import json
import logging
import socket
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path


DEFAULT_GRID_SIZES = [100, 250, 500, 1000, 2000, 3000, 4000, 5000]
DEFAULT_TILE_SHAPES = [
    (8, 64),
    (16, 128),
    (32, 256),
    (64, 512),
    (128, 1024),
]
CHECKSUM_PATTERN = re.compile(r'^checksum:\s+([0-9eE+.\-]+)$', re.MULTILINE)
LOGGER = logging.getLogger(__name__)


@dataclass(frozen=True)
class BenchmarkConfig:
    '''One benchmark configuration for the serial solver.'''

    n: int
    i_block: int
    j_block: int


@dataclass(frozen=True)
class BenchmarkResult:
    '''Timing and correctness data for one benchmark configuration.'''

    config: BenchmarkConfig
    checksum: str
    mean_seconds: float
    stddev_seconds: float
    min_seconds: float
    max_seconds: float
    relative_speedup: float


@dataclass(frozen=True)
class RepositoryInfo:
    '''Repository metadata to include in the benchmark report.'''

    is_git_repo: bool
    is_clean: bool
    remote_url: str | None
    commit_hash: str | None


@dataclass(frozen=True)
class HostInfo:
    '''Host metadata to include in the benchmark report.'''

    hostname: str
    cpu_count: str | None
    architecture: str | None
    model_name: str | None
    threads_per_core: str | None
    cores_per_socket: str | None
    sockets: str | None
    numa_nodes: str | None


def parse_args() -> argparse.Namespace:
    '''Parse command-line arguments.'''

    parser = argparse.ArgumentParser(
        description='Benchmark heat_serial_c.exe over grid and tile sizes.',
    )
    parser.add_argument(
        '--build-dir',
        type=Path,
        default=Path('build_benchmark_serial'),
        help='CMake build directory. Default: build_benchmark_serial',
    )
    parser.add_argument(
        '--report',
        type=Path,
        default=Path('BENCHMARK_HEAT_SERIAL_REPORT.md'),
        help='Markdown report path.',
    )
    parser.add_argument(
        '--grid-sizes',
        default=','.join(str(size) for size in DEFAULT_GRID_SIZES),
        help='Comma-separated grid sizes for -n.',
    )
    parser.add_argument(
        '--time-steps',
        type=int,
        default=100,
        help='Number of time steps passed as -t. Default: 100',
    )
    parser.add_argument(
        '--runs',
        type=int,
        default=6,
        help='Measured runs per configuration. Default: 6',
    )
    parser.add_argument(
        '--warmup',
        type=int,
        default=1,
        help='Warmup runs per configuration. Default: 1',
    )
    parser.add_argument(
        '--hyperfine',
        type=Path,
        default=Path('tools/hyperfine/bin/hyperfine'),
        help='Path to hyperfine. Falls back to PATH lookup if missing.',
    )
    parser.add_argument(
        '--log-level',
        default='INFO',
        choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'],
        help='Logging level for benchmark progress output. Default: INFO',
    )
    return parser.parse_args()


def configure_logging(log_level: str) -> None:
    '''Configure logging for benchmark progress messages.'''

    logging.basicConfig(
        level=getattr(logging, log_level),
        format='[%(levelname)s] %(message)s',
    )


def run_logged_command(command: list[str], description: str) -> None:
    '''Run a subprocess and forward its output through logging.'''

    completed = subprocess.run(
        command,
        check=False,
        capture_output=True,
        text=True,
    )

    if completed.stdout:
        for line in completed.stdout.splitlines():
            LOGGER.info('%s', line)
    if completed.stderr:
        for line in completed.stderr.splitlines():
            LOGGER.warning('%s', line)

    if completed.returncode != 0:
        msg = (
            f'{description} failed with return code '
            f'{completed.returncode}'
        )
        raise subprocess.CalledProcessError(
            completed.returncode,
            command,
            output=completed.stdout,
            stderr=completed.stderr,
        )


def run_capture(command: list[str]) -> subprocess.CompletedProcess[str]:
    '''Run a command and capture its text output.'''

    return subprocess.run(
        command,
        check=False,
        capture_output=True,
        text=True,
    )


def parse_grid_sizes(raw_sizes: str) -> list[int]:
    '''Parse a comma-separated grid-size list.'''

    grid_sizes: list[int] = []
    for item in raw_sizes.split(','):
        value = item.strip()
        if not value:
            continue
        grid_size = int(value)
        if grid_size < 2:
            msg = f'Grid size must be at least 2, got {grid_size}'
            raise ValueError(msg)
        grid_sizes.append(grid_size)

    if not grid_sizes:
        raise ValueError('At least one grid size is required.')
    return grid_sizes


def resolve_hyperfine(hyperfine_path: Path) -> Path:
    '''Resolve the hyperfine executable location.'''

    if hyperfine_path.is_file():
        return hyperfine_path.resolve()

    discovered = shutil.which('hyperfine')
    if discovered is None:
        msg = (
            'hyperfine not found. Install it with '
            'skills/scientific-cli-benchmark/scripts/install_hyperfine.sh'
        )
        raise FileNotFoundError(msg)
    return Path(discovered).resolve()


def get_repository_info() -> RepositoryInfo:
    '''Collect repository metadata when the worktree is clean.'''

    inside_work_tree = run_capture(
        ['git', 'rev-parse', '--is-inside-work-tree']
    )
    if inside_work_tree.returncode != 0:
        return RepositoryInfo(
            is_git_repo=False,
            is_clean=False,
            remote_url=None,
            commit_hash=None,
        )

    status = run_capture(['git', 'status', '--porcelain'])
    is_clean = status.returncode == 0 and not status.stdout.strip()
    if not is_clean:
        return RepositoryInfo(
            is_git_repo=True,
            is_clean=False,
            remote_url=None,
            commit_hash=None,
        )

    remote_url = None
    remote = run_capture(['git', 'config', '--get', 'remote.origin.url'])
    if remote.returncode == 0 and remote.stdout.strip():
        remote_url = remote.stdout.strip()
    else:
        remote_name = run_capture(['git', 'remote'])
        if remote_name.returncode == 0 and remote_name.stdout.strip():
            first_remote = remote_name.stdout.splitlines()[0].strip()
            remote_value = run_capture(
                ['git', 'config', '--get', f'remote.{first_remote}.url']
            )
            if remote_value.returncode == 0 and remote_value.stdout.strip():
                remote_url = remote_value.stdout.strip()

    commit_hash = None
    commit = run_capture(['git', 'rev-parse', 'HEAD'])
    if commit.returncode == 0 and commit.stdout.strip():
        commit_hash = commit.stdout.strip()

    return RepositoryInfo(
        is_git_repo=True,
        is_clean=True,
        remote_url=remote_url,
        commit_hash=commit_hash,
    )


def parse_lscpu_output(output: str) -> dict[str, str]:
    '''Parse lscpu key-value output into a dictionary.'''

    values: dict[str, str] = {}
    for line in output.splitlines():
        if ':' not in line:
            continue
        key, value = line.split(':', maxsplit=1)
        values[key.strip()] = value.strip()
    return values


def get_host_info() -> HostInfo:
    '''Collect basic host metadata for the benchmark report.'''

    hostname = socket.gethostname()

    cpu_count = None
    nproc = run_capture(['nproc'])
    if nproc.returncode == 0 and nproc.stdout.strip():
        cpu_count = nproc.stdout.strip()

    architecture = None
    model_name = None
    threads_per_core = None
    cores_per_socket = None
    sockets = None
    numa_nodes = None

    lscpu = run_capture(['lscpu'])
    if lscpu.returncode == 0 and lscpu.stdout.strip():
        info = parse_lscpu_output(lscpu.stdout)
        architecture = info.get('Architecture')
        model_name = info.get('Model name')
        threads_per_core = info.get('Thread(s) per core')
        cores_per_socket = info.get('Core(s) per socket')
        sockets = info.get('Socket(s)')
        numa_nodes = info.get('NUMA node(s)')

    return HostInfo(
        hostname=hostname,
        cpu_count=cpu_count,
        architecture=architecture,
        model_name=model_name,
        threads_per_core=threads_per_core,
        cores_per_socket=cores_per_socket,
        sockets=sockets,
        numa_nodes=numa_nodes,
    )


def ensure_build(build_dir: Path) -> Path:
    '''Configure and build the serial heat executable.'''

    LOGGER.info('Configuring release build in %s', build_dir)
    run_logged_command(
        [
            'cmake',
            '-S',
            '.',
            '-B',
            str(build_dir),
            '-DCMAKE_BUILD_TYPE=Release',
        ],
        'CMake configure',
    )
    LOGGER.info('Building heat_serial_c.exe')
    run_logged_command(
        [
            'cmake',
            '--build',
            str(build_dir),
            '--target',
            'heat_serial_c.exe',
        ],
        'CMake build',
    )
    executable = build_dir / 'heat_serial_c.exe'
    if not executable.is_file():
        msg = f'Expected executable not found: {executable}'
        raise FileNotFoundError(msg)
    return executable.resolve()


def derive_tile_shapes(n: int) -> list[tuple[int, int]]:
    '''Choose a compact, sensible tile sweep for one grid size.'''

    interior_extent = max(1, n - 2)
    candidates = list(DEFAULT_TILE_SHAPES)
    candidates.append((interior_extent, interior_extent))

    shapes: list[tuple[int, int]] = []
    seen: set[tuple[int, int]] = set()
    for i_block, j_block in candidates:
        clamped = (
            max(1, min(i_block, interior_extent)),
            max(1, min(j_block, interior_extent)),
        )
        if clamped not in seen:
            seen.add(clamped)
            shapes.append(clamped)
    return shapes


def run_checksum(
    executable: Path,
    config: BenchmarkConfig,
    time_steps: int,
) -> str:
    '''Run one correctness check and return the checksum string.'''

    completed = subprocess.run(
        [
            str(executable),
            '-n',
            str(config.n),
            '-t',
            str(time_steps),
            '-i',
            str(config.i_block),
            '-j',
            str(config.j_block),
        ],
        check=True,
        capture_output=True,
        text=True,
    )
    match = CHECKSUM_PATTERN.search(completed.stdout)
    if match is None:
        msg = (
            'Could not find checksum in program output for '
            f'n={config.n}, i={config.i_block}, j={config.j_block}'
        )
        raise ValueError(msg)
    return match.group(1)


def benchmark_with_hyperfine(
    hyperfine: Path,
    executable: Path,
    config: BenchmarkConfig,
    time_steps: int,
    warmup: int,
    runs: int,
) -> tuple[float, float, float, float]:
    '''Benchmark one configuration with hyperfine and parse JSON output.'''

    command = (
        f'{executable} -n {config.n} -t {time_steps} '
        f'-i {config.i_block} -j {config.j_block}'
    )

    with tempfile.TemporaryDirectory() as temp_dir:
        json_path = Path(temp_dir) / 'hyperfine.json'
        subprocess.run(
            [
                str(hyperfine),
                '--warmup',
                str(warmup),
                '--runs',
                str(runs),
                '--export-json',
                str(json_path),
                command,
            ],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        payload = json.loads(json_path.read_text())

    result = payload['results'][0]
    stddev = result['stddev']
    if stddev is None:
        stddev = 0.0
    return (
        float(result['mean']),
        float(stddev),
        float(result['min']),
        float(result['max']),
    )


def collect_results(
    hyperfine: Path,
    executable: Path,
    grid_sizes: list[int],
    time_steps: int,
    warmup: int,
    runs: int,
) -> list[BenchmarkResult]:
    '''Collect correctness and timing data for all configurations.'''

    results: list[BenchmarkResult] = []
    total_cases = sum(len(derive_tile_shapes(n)) for n in grid_sizes)
    completed_cases = 0

    for n in grid_sizes:
        tile_shapes = derive_tile_shapes(n)
        LOGGER.info(
            'Starting grid size n=%d with %d tile cases',
            n,
            len(tile_shapes),
        )
        baseline_checksum: str | None = None
        baseline_mean: float | None = None
        for i_block, j_block in tile_shapes:
            config = BenchmarkConfig(n=n, i_block=i_block, j_block=j_block)
            completed_cases += 1
            LOGGER.info(
                'Case %d/%d: validating checksum for n=%d, i=%d, j=%d',
                completed_cases,
                total_cases,
                n,
                i_block,
                j_block,
            )
            checksum = run_checksum(executable, config, time_steps)
            if baseline_checksum is None:
                baseline_checksum = checksum
            elif checksum != baseline_checksum:
                msg = (
                    'Checksum mismatch for '
                    f'n={n}, i={i_block}, j={j_block}: '
                    f'{checksum} != {baseline_checksum}'
                )
                raise ValueError(msg)

            LOGGER.info(
                'Case %d/%d: timing n=%d, i=%d, j=%d',
                completed_cases,
                total_cases,
                n,
                i_block,
                j_block,
            )
            mean, stddev, min_seconds, max_seconds = benchmark_with_hyperfine(
                hyperfine=hyperfine,
                executable=executable,
                config=config,
                time_steps=time_steps,
                warmup=warmup,
                runs=runs,
            )

            if baseline_mean is None:
                baseline_mean = mean
            relative_speedup = baseline_mean / mean

            results.append(
                BenchmarkResult(
                    config=config,
                    checksum=checksum,
                    mean_seconds=mean,
                    stddev_seconds=stddev,
                    min_seconds=min_seconds,
                    max_seconds=max_seconds,
                    relative_speedup=relative_speedup,
                )
            )
        LOGGER.info('Completed grid size n=%d', n)
    return results


def format_table_row(columns: list[str]) -> str:
    '''Format one Markdown table row.'''

    return '| ' + ' | '.join(columns) + ' |'


def build_report(
    executable: Path,
    hyperfine: Path,
    report_path: Path,
    grid_sizes: list[int],
    time_steps: int,
    warmup: int,
    runs: int,
    repository_info: RepositoryInfo,
    host_info: HostInfo,
    results: list[BenchmarkResult],
) -> str:
    '''Render the benchmark results as Markdown.'''

    lines = [
        '# Heat Serial Tiling Benchmark Report',
        '',
        '## Provenance',
        '',
    ]

    if repository_info.is_git_repo and repository_info.is_clean:
        lines.extend(
            [
                f'- git remote: `{repository_info.remote_url or "unknown"}`',
                f'- commit: `{repository_info.commit_hash or "unknown"}`',
                '',
            ]
        )
    elif repository_info.is_git_repo:
        lines.extend(
            [
                '- git metadata omitted because the worktree is not clean.',
                '',
            ]
        )
    else:
        lines.extend(
            [
                '- git metadata unavailable because this is not a git worktree.',
                '',
            ]
        )

    lines.extend(
        [
            '## Host System',
            '',
            f'- hostname: `{host_info.hostname}`',
        ]
    )
    if host_info.cpu_count is not None:
        lines.append(f'- logical CPUs from `nproc`: `{host_info.cpu_count}`')
    if host_info.architecture is not None:
        lines.append(f'- architecture: `{host_info.architecture}`')
    if host_info.model_name is not None:
        lines.append(f'- CPU model: `{host_info.model_name}`')
    if host_info.threads_per_core is not None:
        lines.append(
            f'- threads per core: `{host_info.threads_per_core}`'
        )
    if host_info.cores_per_socket is not None:
        lines.append(
            f'- cores per socket: `{host_info.cores_per_socket}`'
        )
    if host_info.sockets is not None:
        lines.append(f'- sockets: `{host_info.sockets}`')
    if host_info.numa_nodes is not None:
        lines.append(f'- NUMA nodes: `{host_info.numa_nodes}`')

    lines.extend(
        [
            '',
            '## Overview',
            '',
            'This report summarizes serial benchmark results for '
            f'`{executable}`.',
            '',
            'The benchmark sweep varied:',
            '',
            f'- grid sizes `-n`: {", ".join(str(size) for size in grid_sizes)}',
            f'- time steps `-t`: `{time_steps}`',
            '- tile shapes `(-i, -j)`: '
            '`(8, 64)`, `(16, 128)`, `(32, 256)`, `(64, 512)`, '
            '`(128, 1024)`, plus an untiled full-interior case per grid size',
            f'- warmup runs per case: `{warmup}`',
            f'- measured runs per case: `{runs}`',
            f'- timing tool: `{hyperfine}`',
            '',
            'All tile shapes were validated against a checksum for each grid '
            'size.',
            '',
            '## Results',
            '',
        ]
    )

    for n in grid_sizes:
        n_results = [result for result in results if result.config.n == n]
        best_result = min(n_results, key=lambda result: result.mean_seconds)
        lines.extend(
            [
                f'### n = {n}',
                '',
                f'Checksum: `{n_results[0].checksum}`',
                '',
                format_table_row(
                    [
                        'i block',
                        'j block',
                        'mean [s]',
                        'stddev [s]',
                        'min [s]',
                        'max [s]',
                        'speedup vs first case',
                    ]
                ),
                format_table_row(
                    ['---:', '---:', '---:', '---:', '---:', '---:', '---:']
                ),
            ]
        )
        for result in n_results:
            lines.append(
                format_table_row(
                    [
                        str(result.config.i_block),
                        str(result.config.j_block),
                        f'{result.mean_seconds:.6f}',
                        f'{result.stddev_seconds:.6f}',
                        f'{result.min_seconds:.6f}',
                        f'{result.max_seconds:.6f}',
                        f'{result.relative_speedup:.3f}',
                    ]
                )
            )
        lines.extend(
            [
                '',
                'Best tile shape for this grid size: '
                f'`-i {best_result.config.i_block} '
                f'-j {best_result.config.j_block}` '
                f'with mean runtime `{best_result.mean_seconds:.6f} s`.',
                '',
            ]
        )

    lines.extend(
        [
            '## Summary',
            '',
            format_table_row(['n', 'best i', 'best j', 'best mean [s]']),
            format_table_row(['---:', '---:', '---:', '---:']),
        ]
    )
    for n in grid_sizes:
        n_results = [result for result in results if result.config.n == n]
        best_result = min(n_results, key=lambda result: result.mean_seconds)
        lines.append(
            format_table_row(
                [
                    str(n),
                    str(best_result.config.i_block),
                    str(best_result.config.j_block),
                    f'{best_result.mean_seconds:.6f}',
                ]
            )
        )

    report = '\n'.join(lines) + '\n'
    report_path.write_text(report)
    return report


def main() -> int:
    '''Build the executable, run the sweep, and write the report.'''

    args = parse_args()
    configure_logging(args.log_level)
    grid_sizes = parse_grid_sizes(args.grid_sizes)
    LOGGER.info('Resolving hyperfine executable from %s', args.hyperfine)
    hyperfine = resolve_hyperfine(args.hyperfine)
    LOGGER.info('Using hyperfine at %s', hyperfine)
    repository_info = get_repository_info()
    if repository_info.is_git_repo and not repository_info.is_clean:
        LOGGER.warning(
            'Git worktree is not clean; repository provenance will be '
            'omitted from the report.'
        )
    host_info = get_host_info()
    executable = ensure_build(args.build_dir)
    LOGGER.info('Using executable %s', executable)
    results = collect_results(
        hyperfine=hyperfine,
        executable=executable,
        grid_sizes=grid_sizes,
        time_steps=args.time_steps,
        warmup=args.warmup,
        runs=args.runs,
    )
    build_report(
        executable=executable,
        hyperfine=hyperfine,
        report_path=args.report,
        grid_sizes=grid_sizes,
        time_steps=args.time_steps,
        warmup=args.warmup,
        runs=args.runs,
        repository_info=repository_info,
        host_info=host_info,
        results=results,
    )
    LOGGER.info('Wrote report to %s', args.report)
    return 0


if __name__ == '__main__':
    sys.exit(main())
