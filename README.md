# LumaRPL

A colour scientific and programmable calculator for the ClockworkPi PicoCalc
with an official Raspberry Pi Pico 2 W. Built on DB48X, with RPL, native graphing,
mathematical workbenches, Lua and CHIP-8 programming, SD documents, Wi-Fi time
synchronization and cached currency conversion.

Calculator design direction and device testing: **Iain Bennett**.

## Status

Version `0.10.0-rc.1` packages the port 30.5 calculator with the original evaluator.
This is a release candidate. See [release review](docs/RELEASE-REVIEW.md) for
verified checks, limitations and pending physical acceptance.

## Install

Use the PicoCalc **UF2 Loader v2.4.1** workflow. Copy the release bundle's
`pico2-apps/LumaRPL-Pico-2-W.uf2` and merge its `db48x/` resources into the SD
card. Preserve your existing files and other applications. Select LumaRPL in
the loader. Do not flash the application at an arbitrary address: the UF2 is
linked for the loader application partition.

The supported configuration includes PicoCalc PSRAM. The calculator arena and
stack remain in internal SRAM; PSRAM holds bulk documents and graph data.
Without an SD card, calculation remains available; saving and SD resources do not.

## Controls and help

Boot opens the calculator stack. **Ctrl+T** opens Home. Native menus use
Arrows/Enter, numbered visible rows and labelled F-key actions. Esc goes back.
Calculator soft menus use Tab to choose a row, F1–F5 for its first five entries,
and Ctrl+F5 for its sixth. Ctrl+A opens the command catalog; Ctrl+R opens
Advanced RPL settings; Ctrl+Y opens bases.

The on-device Help & about menu contains calculator, RPL, Lua and CHIP-8
references with examples. Editor Tools provides language reference and examples.

## Development

See [BUILDING](BUILDING.md), [contributing](CONTRIBUTING.md),
[release checks](docs/RELEASE-CHECKLIST.md) and [license notices](NOTICE.md).
Pinned dependencies and the complete upstream patch are fetched by bootstrap;
they are deliberately excluded from this Git repository.

Historical development notes are in [docs/history](docs/history). They describe
past ports and are not the current build instructions.
