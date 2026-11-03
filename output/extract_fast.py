#!/usr/bin/env python3
import os, stat, sys, zipfile

def extract_zip(zip_path, dest_dir="."):
    os.makedirs(dest_dir, exist_ok=True)
    with zipfile.ZipFile(zip_path, "r") as zf:
        infos = zf.infolist()
        total = len(infos)
        for idx, zi in enumerate(infos, 1):
            out_path = os.path.join(dest_dir, zi.filename)
            mode = (zi.external_attr >> 16) & 0xFFFF
            if stat.S_ISLNK(mode):
                target = zf.read(zi).decode("utf-8")
                os.makedirs(os.path.dirname(out_path), exist_ok=True)
                if os.path.lexists(out_path): os.remove(out_path)
                os.symlink(target, out_path)
            elif zi.is_dir() or zi.filename.endswith("/"):
                os.makedirs(out_path, exist_ok=True)
            else:
                os.makedirs(os.path.dirname(out_path), exist_ok=True)
                with zf.open(zi) as src, open(out_path, "wb") as dst:
                    while buf := src.read(1048576): dst.write(buf)
                if mode: os.chmod(out_path, mode & 0o7777)
            if idx % 5000 == 0 or idx == total:
                print(f"  Extracted {idx}/{total} files...", flush=True)

if __name__ == "__main__":
    extract_zip(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else ".")
