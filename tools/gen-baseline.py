#!/usr/bin/python3

"""
Generate `baseline.json` manifest
for current working dir (recursively)
with no args.

Keep attention: cwd of `gen-baseline.py`
should be different (not inside) the dir
you want to be explored and `hashsum`ed.
"""

import hashlib
import json
import mmap
import os


def sha256sum(filename):
    h = hashlib.sha256()
    try:
        st = os.stat(filename)
        if st.st_size == 0:
            return "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"

        with open(filename, "rb") as f:
            with mmap.mmap(f.fileno(), 0, prot=mmap.PROT_READ) as mm:
                h.update(mm)

        return h.hexdigest()
    except Exception:
        return ""


if __name__ == "__main__":
    cwd = os.getcwd()
    print(f"Generate `baseline.json` for files in `{cwd}` (recursively).")

    icl = []
    d = {}

    # obtain integrity check list
    for root, dirs, files in os.walk(cwd):
        for file in files:
            reldir = os.path.relpath(root, cwd)
            relfile = os.path.join(reldir, file)
            icl.append(relfile)

    # calculate hash, transform relative path to non-ordinary form
    for relfile in icl:
        absfile = os.path.join(cwd, relfile)
        checksum = sha256sum(absfile)
        path = relfile.removeprefix('./')
        d[path] = checksum

    # dump to JSON seen by humans (line by line, with indentation)
    with open('baseline.json', 'w') as out:
        json.dump(d, out, indent=4)
