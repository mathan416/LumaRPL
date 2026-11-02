# LumaRPL port 19.4 — visible controls and list navigation

Currency conversion now shows Convert, Amount, From, To, Swap, Refresh rates over Wi-Fi, Manage rates / settings, and Rate details / help together. There is no second currency action page. Manage rates / settings contains import, manual rates, cache saving, Wi-Fi setup and update/date options. After a successful refresh, the main screen returns with Convert selected; the new rates are immediately usable. Failed refresh retains the previous rates and the Refresh selection.

Other action menus were checked for the same problem:

- Formula forms show Result units on the main screen. Examples / help groups the reference actions.
- Parametric/polar controls show Save / load paths on the main screen, with Examples / help together.
- RPL editor Tools shows Debug without paging. Shortcuts and File details are under Help / file details. Back from that submenu returns to Tools.

All registered native action menus now fit within eight rows. Lists of currencies, units, timezones, reference topics and notebook cells can still span pages. These lists show the item range and page count, with labelled F2 Previous / F3 Next buttons when another page is available. Left/Right also works. Number keys 1–8 continue to select visible rows, not absolute entries across pages.

File browsers show the item range, page count and Left/Right cue when more than eight files exist. Run, New, Export and Debug retain their existing labelled keys. The cue covers typed document browsers, RPL/Lua/CHIP-8 files, HOME programs/variables, graph scenes and workbenches.

F4 Details and the built-in PicoCalc reference describe the updated structure. Calculator RPL soft menus keep Tab row selection, F1–F5 columns and Ctrl+F5 for column six. Native pagination does not change these calculator controls.

## Focused device checks

1. Ctrl+U > Cached currency rates: verify all eight main controls are visible. Set Amount, From and To, then Convert. Refresh rates, dismiss the result and verify Convert is selected and the chosen currency codes are retained when present in the new table. Convert immediately.
2. Open Manage rates / settings. Verify import, manual entry, saving, Wi-Fi and update/date options. Esc returns to the main currency controls.
3. In From or To, use F3 Next and F2 Previous. Check the page/range cue, arrow paging, selection and visible-row number shortcuts. The unavailable paging button should not act.
4. Open a formula form: Result units is visible on the main screen. Check Examples / help and Back. Check the corresponding group in Parametric / polar.
5. Open an RPL source editor > F4 Tools: Debug is row 8. Open Help / file details, then Back: Tools should remain open. Read shortcuts or full file details without losing source or cursor state.
6. Browse a folder with more than eight files. The last filename and page cue must not overlap. Use Right to reach later files; verify F2 Run/F3 New still have their original file-browser meaning.
7. Check Reference topics, units, timezones and notebook cells for clear page navigation. Try the changed screens in your preferred themes, then normal calculation, save/reload and loader return.

Host and visual evidence is in reports/PORT194.md. Physical acceptance remains pending; SD deployment is separate. Existing formats, calculator mathematics and internal memory reservations are retained.
