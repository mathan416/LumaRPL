# LumaRPL 0.10.0-rc.2 update checks

Date: 2026-11-01. This release candidate adds the manuals, public installation/development documentation, corrected help descriptions and complete font/component acknowledgements to rc.1. Calculator mathematics, the original evaluator and saved formats are unchanged.

## Fresh update checks

- Pico 2 W Arm build: passed.
- Actual SD-card UF2 Loader v2.4.1 metadata/address/partition validation: passed.
- Production controller suite: passed, 325 screens, 29,557 keys, 32,230 frames.
- All 309 shipped calculator/RPL help examples: passed against the production engine.
- Generated help/reference/font/insertion checks and UI rendering audit: passed.
- License-bundle helper: copied notices checked byte-for-byte.
- README local links and manual SHA-256 checks: passed.
- Manual host examples: 126 passed; all 143 PDF pages visually reviewed. Manuals identify the rc.1 baseline; this update changes documentation/credits only.

Flash image: 2,273,192 bytes of 4,186,112. Static internal RAM: 390,276 bytes; calculator arena: 212,992; bounded heap: 98,304; stack: 32,768; framebuffer: 102,400; shared workspace: 16,384; unallocated gap: 2,940. Arena/framebuffer/workspace are included in static RAM. Reservations are unchanged.

Firmware SHA-256:
`b7095cbd6c1651a10216a4d48124dae142a59e6151edfd4ff5bf2201362a8824`

The rc.1 review retains the full upstream-suite, sanitizer, allocation/TLS and focused visual baseline. Those larger checks were not rerun for this documentation-only update. The fresh update checks above are reported separately.

## Physical acceptance

Still pending for this exact candidate: cold boot, updated help/credits, save/restart recovery, loader return and mixed use. Build, publication and copying to SD do not establish device acceptance. No final stable release is claimed.
