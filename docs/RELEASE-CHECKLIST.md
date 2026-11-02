# Release checklist

- Bootstrap a fresh checkout; verify every source pin and patch hash.
- Build firmware with the documented complete toolchain; validate UF2 family,
  addresses and loader partition fit. Record the linker memory report.
- Run host sanitizers, production controllers, Lua and Arm allocation/TLS checks.
- Compare a fresh unchanged upstream full suite with the patched full suite.
  Explain every new failure and assertion before release.
- Inspect changed screens and record theme/capture coverage.
- Check licenses, source correspondence and bundle checksums; scan candidate
  tracked files for credentials and accidental user data.
- Cold boot on hardware; test stack/soft menus, all Home categories, calculation,
  graphs/interruption, RPL/Lua/CHIP-8, editor recovery, save/restart, Wi-Fi/NTP,
  immediate and cached currency conversion, and loader return.
- Complete a 30-minute mixed-use device session. Record firmware hash and results.
- Tag only the verified commit. Publishing and SD deployment are explicit actions.

Keep host-verified, visually reviewed and device-verified status separate.
