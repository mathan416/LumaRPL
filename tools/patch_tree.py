"""Check that the db48x checkout is the pinned upstream plus the verified patch."""
import subprocess
from pathlib import Path

# The upstream simulator build regenerates these tracked documentation files;
# packaging takes them from upstream HEAD, so they never reach a release.
REGENERATED = {'doc/8-menus-tree-dm42.md', 'help/db48x.md'}
# Untracked files here would be compiled or embedded into the firmware.
BUILD_INPUTS = ('src', 'fonts')


def unexpected_changes(source, manifest):
    """Return db48x paths that differ from upstream HEAD outside the patch.

    Firmware is built from the working tree, while the published source is
    upstream HEAD plus the patch files, so any other change would make the
    source no longer correspond to the binary.
    """
    source = Path(source)
    patched = set(manifest['files'])
    changed = set(subprocess.check_output(
        ['git', 'diff', '--name-only', 'HEAD'], cwd=source, text=True).splitlines())
    added = set(subprocess.check_output(
        ['git', 'ls-files', '--others', '--exclude-standard', '--', *BUILD_INPUTS],
        cwd=source, text=True).splitlines())
    return sorted((changed - REGENERATED | added) - patched)
