import glob
import os
import subprocess
import sys
import threading

def format_files(files):
    blacklist = ["sound_data.tbl.inc.c", "stb_image.h"]
    blacklist_dirs = ["anims", "third_party_libs", "build"]
    for file in files:
        if os.path.basename(file) not in blacklist and not any(blacklist_dir in file for blacklist_dir in blacklist_dirs):
            print(f"Formatting {file}...")
            subprocess.run(["clang-format", "-i", "-style=file", file], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        else:
            print(f"Skipping {file}...")

if len(sys.argv) > 1:
    print(f"Formatting file(s) {sys.argv[1:]}")
    format_thread = threading.Thread(target=format_files, args=(sys.argv[1:],))
    format_thread.start()
    format_thread.join()
    print("Done.")
else:
    print("Formatting...")
    files_to_format = glob.glob("**/*.[ch]", recursive=True) + glob.glob("**/*.cpp", recursive=True) + glob.glob("**/*.hpp", recursive=True)
    format_thread = threading.Thread(target=format_files, args=(files_to_format,))
    format_thread.start()
    format_thread.join()
    print("Done.")