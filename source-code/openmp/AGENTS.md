# Repository Instructions

## Scope

These instructions apply to the OpenMP examples under `source-code/openmp`.
Keep this file focused on guidance that is broadly useful across these
directories. Put project-specific instructions in a deeper `AGENTS.md` only
when a particular example genuinely needs exceptions or extra requirements.

## Project Context

These examples are part of a parallel-programming and scientific-computing
codebase. Prefer solutions that are straightforward to benchmark, validate, and
reason about in HPC settings.

## Coding Expectations

- Prefer modern language standards and idioms.
- Keep source lines to 79 characters when practical.
- Use descriptive names.
- Avoid unnecessary dependencies and includes/imports.

## Validation

- Use appropriate static analysis tools when they are available.
- For Python, prefer `pylint`, `flake8`, and `mypy`.
- For C++, prefer `clang-tidy` and `cppcheck`.

## Testing

- Provide automated tests for major functionality when the repository structure
  supports them.
- Cover typical cases and important edge cases.
- Use standard test frameworks for the language in use.
- For Python, prefer `pytest`.
- For C++, prefer Google Test.

## Documentation

- Document nontrivial functions, modules, and classes.
- Add usage examples when they materially help readers understand behavior.
