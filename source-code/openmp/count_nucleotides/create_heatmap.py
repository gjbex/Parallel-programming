#!/usr/bin/env python

import matplotlib.pyplot as plt
import numpy as np

data = np.genfromtxt('benchmark_2.csv', delimiter=',', names=True)

threads = np.unique(data['parameter_threads'])
buffer_sizes = np.unique(data['parameter_buffer'])
median_times = data['median']

# create a meshgrid
Threads, Buffer_sizes = np.meshgrid(threads, buffer_sizes)
Median_times = median_times.reshape(len(buffer_sizes), len(threads))

# compute grid values for single thread
Median_1 = np.tile(Median_times[:, 0].reshape(len(buffer_sizes), 1), (1, 6)).reshape(len(buffer_sizes), len(threads))

# compute efficiency and average efficientcy for each number of threads
Efficiency = Median_1/(Median_times*Threads)
Avg_effiiciency = np.tile(np.mean(Efficiency, axis=0).reshape(1, len(threads)), (len(buffer_sizes), 1))

# plot heatmap
plt.pcolormesh(Threads, Buffer_sizes, (Efficiency - Avg_effiiciency)/Avg_effiiciency, cmap='bwr')
plt.colorbar()
plt.xlabel('Threads')
plt.ylabel('Buffer size')
plt.title('Relative efficiency')
plt.savefig('heatmap.png')
