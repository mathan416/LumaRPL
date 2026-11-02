"""Copy maintained notices to the SD-visible location named by Credits."""
from pathlib import Path
import shutil

def copy_license_bundle(root: Path, destination: Path):
    shutil.copytree(root/'licenses',destination,dirs_exist_ok=True)
    shutil.copy2(root/'NOTICE.md',destination/'PORT-NOTICES.md')
    shutil.copy2(root/'SOURCES.lock.json',destination/'SOURCES.lock.json')
