# Manual validation and credits audit

First edition, November 2026. Baseline: 0.10.0-rc.1, port 30.5, original evaluator.

## Delivery

- User Guide: 60 pages; calculator tutorials, workbenches, saving, shortcuts and troubleshooting.
- Programming Guide: 83 pages; editor workflows, RPL tutorials, supported Lua API and CHIP-8 assembly examples.
- Both include the supplied cover photograph, linked contents, PDF bookmarks, alphabetical index, credits and embedded-font notices.

## Verification

All 143 rendered pages were visually inspected through numbered contact sheets. Covers and representative body/table pages were also inspected individually. Code-block splitting was corrected during review. Automated page-boundary checks report zero out-of-page characters. PDF outlines, links and text extraction were checked.

126 host example cases passed: 29 calculator/RPL, 51 Lua and 46 CHIP-8. These use production engine/runtime/assembler/VM implementations, with host device I/O and synthetic VM keys/timers. The production UI controller suite passed 325 screens. Generated help and rendering audits passed. This is not a new physical-device acceptance claim.

Machine-readable build, example, layout and validation records plus SHA256SUMS accompany the PDFs in output/pdf/. The sources and build requirements are retained here; generated artifacts are ignored by Git.

## Attribution corrections

Added missing editor-font acknowledgements to the on-device credits source. Updated obsolete port/report references in NOTICE.md. Retained full direct-component notices, including DB48X, Pico SDK, UF2 Loader, SwissMicros declarations, FatFS and Lua. The packaging helper now copies notices into /db48x/licenses/, matching the location advertised by Credits. Its copied files were checked byte-for-byte against their sources.

The manuals distinguish runtime components from development tools, external rate data and PDF-only fonts. Pico SDK supplies Pico printf; this build selects compiler/Newlib printf. HP manuals are linked as historical design inspiration and further reading; no HP prose or artwork was reproduced.

Source help corrections describe existing behaviour: direct integer base conversion, the /db48x/datasets/ CSV directory and current calculus navigation. The installed help, firmware and SD card have not been replaced. These source corrections belong in the next requested firmware release.
