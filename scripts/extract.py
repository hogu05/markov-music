#!/usr/bin/env python3
import argparse
import json
import os
import shutil


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("composer")
    parser.add_argument("output")
    parser.add_argument(
        "--dataset",
        default="aria-midi",
    )
    args = parser.parse_args()

    metadata_path = os.path.join(args.dataset, "metadata.json")
    data_path = os.path.join(args.dataset, "data")

    with open(metadata_path) as f:
        metadata = json.load(f)

    matches = [
        id_
        for id_, entry in metadata.items()
        if args.composer.lower()
        in entry.get("metadata", {}).get("composer", "").lower()
    ]

    os.mkdir(args.output)

    count = 0
    for id_ in matches:
        filename = f"{int(id_):06d}_0.mid"
        for sub in os.listdir(data_path):
            path = os.path.join(data_path, sub, filename)
            if os.path.exists(path):
                shutil.copy2(path, os.path.join(args.output, filename))
                count += 1
                break

    print(f"Extracted {count} files")
    return 0


if __name__ == "__main__":
    main()
