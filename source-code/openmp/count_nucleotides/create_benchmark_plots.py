#!/usr/bin/env python

import argparse
import pandas as pd
import pathlib
import matplotlib.pyplot as plt

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('benchmark_file', type=str, help='Benchmark file')
    args = parser.parse_args()

    benchmark_file = pathlib.Path(args.benchmark_file)
    base_name = benchmark_file.stem

    # create plot for runtime
    figure = plt.figure()
    axis = figure.add_subplot(111)
    df = pd.read_csv(args.benchmark_file, sep=',')
    df.plot.scatter(x='parameter_threads', y='median', rot=0, figsize=(20, 10),
            yerr=(df['median'] - df['min'], df['max'] - df['median']), ax=axis)
    axis.set_xlim(df['parameter_threads'].min(), df['parameter_threads'].max())
    plt.savefig(f'times_{base_name}.png')

    # create plot for speedup
    figure = plt.figure()
    axis = figure.add_subplot(111)
    df['speedup'] = df['median'].iloc[0]/df['median']
    df.plot.scatter(x='parameter_threads', y='speedup', ax=axis)
    axis.set_xlim(df['parameter_threads'].min(), df['parameter_threads'].max())
    plt.savefig(f'speedup_{base_name}.png')

    # create plot for efficiency
    figure = plt.figure()
    axis = figure.add_subplot(111)
    df['efficiency'] = df['speedup']/df['parameter_threads']
    df.plot.scatter(x='parameter_threads', y='efficiency', ax=axis)
    axis.set_xlim(df['parameter_threads'].min(), df['parameter_threads'].max())
    axis.set_ylim(0.0, 1.0)
    plt.savefig(f'efficiency_{base_name}.png')

if __name__ == '__main__':
    main()
