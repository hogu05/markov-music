#!/usr/bin/env python3
import argparse
import os
import random
import shutil


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("source")
    parser.add_argument("dest")
    parser.add_argument("--count", type=int, default=20)
    args = parser.parse_args()

    files = os.listdir(args.source)
    sample = random.sample(files, args.count)

    os.mkdir(args.dest)

    for filename in sample:
        shutil.move(
            os.path.join(args.source, filename), os.path.join(args.dest, filename)
        )


if __name__ == "__main__":
    main()
