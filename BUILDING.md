# Build LumaRPL

Use Python 3.12 or newer, Git, CMake 3.20+, Ninja, a complete Arm GNU Embedded
compiler (C and C++ plus newlib), and Qt 6 with qmake for host integration tests.
Clang and its address/undefined-behaviour sanitizers are used by the host tests.
The firmware baseline uses Arm GNU Embedded **9.2.1**, Pico SDK **2.2.0**,
Lua **5.4.9** and Mbed TLS **3.6.7**. Exact source revisions are in SOURCES.lock.json.
QEMU is required for the separate Arm allocation tests.

```sh
python3 tools/bootstrap.py
# Put the complete embedded toolchain first in PATH. For example:
# export PATH=/path/to/arm-toolchain/bin:$PATH
sh tools/build.sh
```

`PICO_TOOLCHAIN_PATH` may be supplied to CMake when multiple compilers are installed:
`sh tools/build.sh -DPICO_TOOLCHAIN_PATH=/path/to/arm-toolchain`.
A compiler installation without newlib headers cannot build this application.
Use a fresh build directory after changing compilers.

A release source archive already includes dependency working trees; build it
directly with `sh tools/build.sh`. Bootstrap is for Git checkouts and expects
Git metadata for pinned dependencies.

Bootstrap verifies dependency commits, required submodule commits, the complete
patch SHA-256 and all patched file hashes. It refuses to overwrite conflicting
patches. Optional `--source-cache /path/to/existing/workspace` reuses local Git
objects, never its modified working tree. Lua is authenticated against the
pinned official archive. Network access is required on the first bootstrap.

## Verification

```sh
sh tools/test.sh
sh tools/test_lua.sh
python3 tools/build_colour_simulator.py --indexed
python3 tools/test_surface_engine.py --indexed
python3 tools/test_controllers.py
python3 tools/test_help_examples.py
python3 tools/render_review.py
python3 tools/verify_ui_captures.py
python3 tools/check_release_resources.py
sh tools/test_arm_allocation.sh
sh tools/test_arm_tls.sh
```

Run these sequentially: simulator/controller probes share build objects.
Controller tests execute production controllers and the upstream engine with
scripted device I/O. Captures and reports go under ignored `reports/` and `build/`.
They do not establish physical device acceptance.

For a fresh upstream comparison, build an unchanged checkout at the locked DB48X
commit and the patched checkout with the same compiler, Qt configuration and
fixtures. Run both full headless suites using `tools/run_upstream_suite.py
--baseline --binary /absolute/path/to/db48x --work /absolute/path/to/isolated-work`.
Use `tools/compare_full_results.py BASELINE.log POST.log` to compare failures and
assertions. Baseline failures must be documented, never silently accepted by
changing expected images.

```sh
python3 tools/package_release.py
```

Packaging requires passing firmware memory/loader checks. Outputs are under
`dist/`; dependency source is included with the source archive. Packaging never
copies to an SD card. See docs/RELEASE-CHECKLIST.md before distributing a release.
