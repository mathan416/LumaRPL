# LumaRPL port 21.3 — help rewritten, one reader

This release replaces the previous help text rather than adding more annotations to it. The guide, contextual pages and menu Details have been written anew. The command reference is a new set of entries for all 1,026 parseable engine commands, menus and settings, grouped into 17 subjects alongside 16 new RPL guide topics. The previous imported manual and annotation pipeline are no longer the on-device reference.

The reference now uses native lists and the same text reader as other help. Arrows select a command; Enter or F1 opens it; 1-8 opens a visible row. Left/Right or labelled Previous/Next changes list pages. F4 Find searches names and accepted spellings, putting an exact match first. In a page, Up/Down scrolls, Left/Right pages and F5/Esc returns. Returning keeps the selected list row and page. There are no six-column help controls, link-selection mode or automatic example insertion.

Open Calculator reference, then Command reference. Ctrl+K opens Find from the stack. The RPL Help command and help requests from the calculator engine open this native reader too. Calculator soft menus still use Tab/F1-F5/Ctrl+F5; only help navigation changed.

The writing explains the immediate task before qualifications. Command pages describe what happens and provide tested examples where available. The review corrected action ordering in several Details pages and documents real limitations, including unavailable RREFMOD and unsupported complex arguments for several special functions. Source files under docs/help are independent of the old manual; upstream engine documentation remains in the source archive for historical/reference purposes.

The native reference is SD-backed. It caches eight visible records and allocates at most 4,097 bytes for a topic, releasing it when the reader closes. Topic lengths, offsets, UTF-8 and CRC are checked before display. Category browsing reads only the page it needs; a name search scans the bounded index. Missing/damaged resources produce a normal Back screen, and the built-in guide still works without SD. No new framebuffer or calculator arena allocation is introduced.

The release also consumes the original held-key release after opening Help, so closing a page cannot accidentally execute that calculator command.

Measured firmware flash use is 2,010,848 bytes, 10,864 fewer than port 21.2. Static SRAM use is 387,628 bytes, 100 fewer; the 208 KiB arena, 96 KiB heap, 32 KiB stack, 100 KiB framebuffer and 16 KiB workspace reservations are unchanged. The unallocated SRAM gap is 5,588 bytes.

Verification reports are under reports/port213-*. They distinguish reference-store ASan/UBSan tests, real-engine examples, production-controller checks, visual review and device acceptance. The rewrite changes documentation and the PicoCalc Help route; it does not change calculator mathematics. A fresh full desktop suite is not claimed for this release.

After a separately requested deployment, check:

1. Open the calculator guide and read a topic, then F5 back.
2. Open Command reference. Browse a subject; open a command; use arrows; F5 should return to the same list position. Esc from the list returns to subjects.
3. Use F4 Find for Root, UIAlpha, StackFont, FixedDisplay and BeepOn. Exact commands should appear first. No old link-navigation controls should appear.
4. Enter the documented SQDEMO program and confirm 12 SQDEMO gives 144. Try unit conversion and the radians root example after saving important calculator state.
5. Open editor, graph, workbench, matrix and Lua Help. Check that the descriptions make sense without translating internal implementation terms.
6. Check readable wrapping in your preferred theme, restart after saving, and verify loader return.

The source, firmware, SD resources, checksums and review captures ship together. Firmware keeps the canonical name pico2-apps/LumaRPL-Pico-2-W.uf2. User data and saved formats are unchanged. SD deployment and physical acceptance remain separate requested steps.
