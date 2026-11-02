# LumaRPL port 21.6 — practical language references

The default calculator header no longer displays “Session”. Custom session names remain visible.

Lua and CHIP-8 references now open with Start here, followed by task sections and command lookup. Lua has 38 pages covering a first script, saving/reopening, drawing, plotting, input and troubleshooting. CHIP-8 has 44 pages covering a first drawing, source files, sprites, keys/timers, instructions and troubleshooting. Each walkthrough gives the PicoCalc controls and an expected result. Complete CHIP-8 programs and instruction fragments are labelled separately. Canonical API and instruction names remain searchable. Each reference remembers its own menu selection.

Help & about ends with About LumaRPL, then Credits / licenses. The calculator reference and other Help content are retained.

## Testing after deployment

1. Restart and check that the default header has no Session label.
2. Open Home → Help & about. Confirm About LumaRPL and Credits / licenses are last.
3. Open Lua reference → Start here → Run your first script. Follow the steps; the script should print 5. Save/reopen it. Try Draw your first picture next.
4. Open CHIP-8 reference → Start here → Run your first CHIP-8 program. Follow the profile-setting steps. The program draws 5 and waits for a key; press and release X to return.
5. Browse Instruction reference and check that fragments are clearly labelled. Find screen.text in Lua and DRW in CHIP-8. Use arrows to page and Esc/F5 to return.

Firmware and SD help resources must be updated together. Deployment and physical acceptance are separate. The release preserves calculator mathematics, saved formats, editor geometry and memory reservations. Host checks cover actual controllers, all language examples and bounded reference loading; they do not establish device appearance or timing. A fresh full desktop simulator suite is not claimed.

## References and examples inside source editors

In an RPL, Lua or CHIP-8 source editor, press F4 Tools. The first two entries are Language reference and Open example. Language reference opens the matching book and returns to the same editor. Open example shows the source and expected result; F1 Open asks for a new filename, then opens a separate unsaved document. Saving uses the normal language folder. Close the example to return to your original text, cursor, selection, dirty flag, clipboard and undo history. Existing filenames are rejected. Another example cannot be opened inside an example; return to the original editor first.

The 48 examples comprise four complete RPL programs, six complete CHIP-8 programs and 38 Lua scripts derived from the help. CHIP-8 examples using EXIT require SUPER-CHIP modern, selected in the CHIP-8 file list's F4 Actions → Compatibility profile. The preview can be read without SD; opening a new document requires SD for its save destination. If memory cannot hold the original safely, the original remains open and the example is not started.

Test all three editors: change a source without saving, open its reference, then open and save an example under a new name. Close the example and check that your original edits and selection remain. Undo should still work. Try cancelling the example, an existing filename and a failed save. Finally use Save & run on a new example and reopen its saved file.
