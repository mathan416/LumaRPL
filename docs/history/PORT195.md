# LumaRPL port 19.5 — remember currency selections

Currency From and To now save automatically when changed, including Swap. No confirmation or separate Save action is needed. The selections restore when reopening Currency conversion and after restarting the calculator.

The pair is saved by three-letter code in `/db48x/currency/PAIR.cfg`, independently of the rate table and calculator session. Rate refresh/import can reorder currencies without changing your selection. If a saved currency is absent, its code stays selected and conversion explains that its rate is unavailable; it never silently converts a substitute pair.

Autosaving uses bounded, versioned, checksummed data and the existing temporary-file/backup recovery path. Choosing the same currency, cancelling, viewing or reopening a saved pair does not rewrite SD. If SD is unavailable or saving fails, the pair remains selected in RAM and a non-modal status explains that it is session-only. Reopening retries saving automatically. Amount remains a per-visit input; this release persists From and To.

## Device checks

1. Ctrl+U > Cached currency rates: choose your From and To currencies. There should be no save prompt. Leave Conversions and reopen: both codes should remain selected.
2. Restart, reload LumaRPL and reopen Currency conversion. Confirm both codes remain, without using Ctrl+S or Save current cache for the selection.
3. Refresh rates. Confirm the same pair remains selected and converts immediately.
4. Use Swap, leave and reopen; restart again to verify the swapped pair was saved.
5. Cancel a From/To picker or select the current code again: the pair should stay unchanged.

Host verification exercises the real production dropdown, autosave, conversion and status controllers in all eight themes, including a reconstructed runtime state for restart, reordered/missing rates, cancellation, unchanged selection, Swap, failed writes and automatic retry. Separate sanitizer tests use actual FatFS for remount/restart, corruption/backup recovery and failed replacement. Device acceptance remains pending.

Static RAM increases by 12 bytes. Calculator arena 208 KiB, bounded heap 96 KiB, stack 32 KiB, RGB332 framebuffer 100 KiB and workspace 16 KiB remain unchanged. No application flash writes are used. Firmware deployment is separate from implementing automatic preference saving.
