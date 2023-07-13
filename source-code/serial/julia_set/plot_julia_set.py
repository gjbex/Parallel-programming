#!/usr/bin/env python

import argparse
import numpy as np
import matplotlib.pyplot as plt

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("filename", help="name of the file to plot")
    args = parser.parse_args()

    data = np.genfromtxt(args.filename, delimiter=' ')
    plt.imshow(data, interpolation='nearest')
    plt.show()

if __name__ == "__main__":
    main()
