# API & Assembly Reference audit

API baseline: LumaRPL 0.10.0-rc.2; unchanged interfaces rechecked for 0.10.0-rc.3.

The 71-page companion covers production Lua APIs, the implemented CHIP-8 and SUPER-CHIP interfaces, and six reusable routines supplied as assembly source. Native Arm development is excluded. Nine complete example files accompany the reference.

## Verification

- All 135 cases in the manual example suite passed: 29 RPL, 55 Lua and 51 CHIP-8.
- Production assembler/VM routine contracts passed AddressSanitizer and UndefinedBehaviorSanitizer checks, including pixels, registers, timers, key handling and failure cases.
- Maintained example files match the complete fenced examples in the reference source.
- All 71 page bodies were visually reviewed. Final changed pages were inspected; unchanged bodies were compared with reviewed renders.
- PDF layout checks found no out-of-page characters or sparse pages. Linked contents, outlines and the index are present.
- The example ZIP matches the maintained sources and includes the project license.

Physical-device execution of these new examples remains unverified. No firmware, SD card, GitHub branch or release was changed by this documentation task.

Reproduce with the API-specific build/review commands and manual example test command in README.md. Generated validation reports and delivery checksums are in output/pdf/.
