# Port 18 memory backend

The current colour target uses a 100 KiB RGB332 framebuffer. Calculator arena
208 KiB, bounded newlib heap 96 KiB, Lua payload cap 48 KiB within that heap,
stack 32 KiB and a shared 16 KiB working buffer all remain internal. Existing
core colour patterns/settings and pixmaps remain RGB565; screen conversions
handle untyped legacy setting values, mirrored rows, glyph alignment prefixes
and overlapping copies.

`psram.c` implements checked byte-addressable storage; `psram_bus.c` and
`psram_spi.pio` drive GPIO2 MOSI, GPIO3 MISO, GPIO20 CS and GPIO21 SCK. GPIO4/5 are
unused extra data lines in SPI mode, configured as quiet inputs. A full-duplex
transaction has at most 13 bytes, fitting both four-word FIFOs before execution.
At 60 MHz PIO, the instruction model bounds CS low at 5.667 us. A stalled state
machine is abandoned after a 50 us CPU wait, CS is forced high, and callers
receive failure. Claims are non-panicking. No DMA, second core, XIP mapping,
interrupt callback, network transport or flash write is used by this backend.

Boot resets the chip, checks its ID response and writes distinct patterns at
power-of-two banks before reading all of them back. This rejects absent/stuck
responses and aliases of smaller chips. The check is a boot qualification, not
an exhaustive retention test. PSRAM faults are separate from calculator GC.

`bulk_storage.h` owns 127 fixed 64 KiB slots after a reserved first slot. Only
reviewed pointer-free value documents/records may be serialized there; the
trivially-copyable assertion is necessary but does not by itself forbid pointers.
Native document guards restore an inactive value on entry and park on exit.
Parking verifies the complete written value in 256-byte chunks before freeing
its internal copy. Failure retains the resident copy. Graph/source blobs validate
checksums on restore; graph failures resample and source failures reload the
explicitly saved SD file. Undo uses a checked record cache, with bounded internal
fallback. Never store GC roots, allocator metadata or live Lua objects in PSRAM.

`workspace.h` grants an exclusive lease to editor, source or plot activity. Native
program execution releases the editor/source lease first so a program may plot.
A busy lease reports failure. No second framebuffer is allocated. Existing state,
scene, document and program formats are unchanged, and all persistent user data
continues to use the established SD replacement/recovery paths.

The sections below document previous ports and may contain historical budgets.

---

# Platform service inventory

The core is pinned DB48X. `pico/` supplies a DMCP-shaped source interface, not
an emulated SwissMicros ROM. `pico/lft_ifc.h` deliberately prevents loading
SwissMicros fixed-address function tables. The final ELF links without any
unresolved symbols. Existing upstream firmware/simulator targets are retained.

| Service group | Implementation / limit |
| --- | --- |
| LCD buffer/address/refresh | 320×320 mirrored RGB565 (or optional one-bit) surface in internal RAM; clipped dirty rows converted to RGB888 scanlines for ClockworkPi's ILI9488 SPI setup |
| Drawing/font services | Upstream blitter plus flash-resident generated Unicode fonts; immutable copies of simulator's six legacy DMCP fonts; unknown legacy font sizes use a supported font |
| Key queue/read/wait | Timed I²C FIFO polling, key translation, synthetic release events, QWERTY text, UTF-8 cursor movement, held-key repeat, Ctrl shortcuts; Esc is visible to upstream interrupt checks |
| Timers/ticks/delay | Pico monotonic milliseconds, four software timers, wrap-safe timer deadlines |
| Runtime memory | Fixed aligned 160 KiB colour GC arena; bounded newlib heap; dedicated 32 KiB stack in main SRAM; stack-limit register; no PSRAM/direct-memory assumptions |
| Files | Writable FatFS R0.16 with SPI SD, source-compatible FIL API; every calculator path rooted in /db48x; explicit write/close error tracking |
| SD write/sync | Adapted v2.4.1 driver uses acknowledged single-sector CMD24 writes and a card-ready synchronization step; monochrome session persistence user-confirmed; colour UI testing pending |
| State/keymaps | Upstream serializer/parser/keymap formats; checked temporary-file promotion with backup and startup fallback; explicit load recovery snapshot; separate colour session with first-boot import |
| Battery | Keyboard register 0x0b, high byte percentage/charging bit; meter and optional percentage text, no actual voltage sensor |
| RTC/calendar | Session-only software clock, date/Julian conversion and formatting; no cold-boot retention or network time |
| Buzzer | PWM speaker at GPIO26; silent flash remains upstream; hardware timing/volume needs verification |
| Idle/power | Polling sleeps and backlight register 0x85; no power-controller firmware changes and no deep sleep |
| Loader | Standard SDK RP2350 Arm Secure UF2; v2.4.1 partition checked offline and queried at runtime; Ctrl+Q uses its watchdog scratch BOOT_SD command after a successful save |
| System setup | Native setup for themes, angles, session actions, programs and graphs; upstream calculator menus retained |
| Screenshots/file chooser | Screenshots and legacy file chooser unavailable; native paged program browser and bounded UTF-8 editor provided |
| Program tools | HOME program run/export, paged SD browser, UTF-8 source editor under 16 KiB, checked save/backup and Save+Run |
| Graph tools | Function entry, viewport presets/custom bounds, pan/zoom, plot colours; direct screen rendering preserves any Pict object |
| SwissMicros flash tools | Erase/write functions return failure; no calculator flash writes, QSPI CRC checks or SwissMicros boot path |
| Critical sections | No-op at the compatibility API: calculator/backend state is single-core; SDK hardware primitives provide their own synchronization |
| Legacy auto-off counters | Disabled; keyboard controller's PMU retains its own battery management |
| Printer/USB/Bluetooth | Unsupported; no app implementation linked |
| Wi-Fi/HTTPS | Port 17: Pico SDK polling CYW43/lwIP, verified on-demand HTTPS reference rates; physical testing pending |

`pico/main.cc`, `sysmenu.cc`, `target.*`, `dmcp.h` and `dmcp_fonts.cc` retain their
upstream copyright/license headers. Mathematical algorithms remain upstream.
The only shared blitter change is a zero-rotation guard found by UBSan; the other
shared source changes are conditional PicoCalc percentage-battery handling and monotonic timing.

A 4 MiB board with the installed v2.4.1 loader provides 4,186,112 application
bytes. Logical XIP 0x10000000 is remapped by the loader to physical offset 8192;
link the application normally rather than shifting its vectors by 8192. The
first SDK UF2 block is an absolute E10 dummy at 0x10ffff00; v2.4.1 skips it and
then validates the RP2350 application blocks. `tools/validate_uf2.py` follows
that contract and reads the actual packaged loader partition table.

## 3D port 3

Native 17x17 wireframe sampling uses pico/surface_evaluator.h with the upstream
2-variable evaluator and temporary X/Y bindings. Scoped hardware-float mode
and 16-digit precision apply only while sampling; display coordinates use
bounded single-precision projection. The double sample mesh is 2312 bytes on
the reserved stack. There is no second framebuffer or heap allocation for it.
The mesh is normalized independently per axis and reused for camera operations.
Domain holes are skipped; sampling and calculation support cancellation.

## Compact native header

PICOCALC settings::header_font() selects a generated 12-pixel FogSans font for
consistent header, battery and annunciator metrics. Other targets retain their
upstream font selection. The standard Session-colour state path displays as
Session; no save path or state format is changed. Generated data adds about
10.4 KiB of flash and no framebuffer or calculator-arena allocation.

## Port 5: programming engines

The independent, bounds-checked CHIP-8 VM owns 4 KiB virtual RAM, 1 KiB physical
128x64 bitplane, registers, 16 call-stack slots, 60 Hz timers and per-ROM flags.
It is allocated from the bounded general heap while active, without using the
calculator arena. The display reuses Screen. Profiles pass Corax+/flags and
published VIP/modern/legacy SUPER-CHIP quirks indicators under host sanitizers.
The keyboard driver exposes individual press/release events; normal calculator
polling still supplies character repeat. Physical rollover remains unverified.

The native assembler is two-pass with bounded labels, source lines and output.
The sprite editor exports original source data. RPL debugging uses the existing
Debug, SingleStep, StepOver, Continue and Kill execution paths, not a second RPL
interpreter. Source syntax diagnostics use the engine error-source pointer
when available. HOME inspection renders bounded, read-only values.

Lua 5.4.9 is linked as C with selected libraries and LUAI_MAXCCALLS=40. Its custom
allocator rejects live payloads above 28 KiB within the existing 64 KiB heap;
allocator metadata and editor buffers also consume that heap. A count hook
polls Esc. C library/VM errors unwind through Lua's protected calls. The C++
calculator bridge returns before Lua pushes a result or raises an error, so
Lua longjmp cannot skip live DB48X GC roots in the bridge. calc.eval accepts a
single algebraic expression and returns its formatted string. Lua's own numeric
arithmetic is separate from DB48X arithmetic. New Lua states are closed on every
run; output/source/nesting bounds and allocation failures are tested.

The compiler/newlib printf implementation supplies C99 formatting required by
Lua string.format; this replaces the SDK lightweight formatter on this target.

## Port 6

Lua's persistent session context owns the state, allocation accounting and an
output buffer. All compile/execute/result-print operations are protected; a
console command can return incomplete without executing a partial chunk. The
existing evaluator remains the calculator bridge. C wrappers validate arguments
before platform drawing and input callbacks return; Lua errors/allocations never
longjmp across live C++ GC roots. Native drawing uses the existing Screen and
restores the clip/graphics state on close. A bounded press queue and held-key
bitset preserve input for keyboard APIs without losing it to the cancel hook.

Editor tools record bounded byte deltas with cursor state, grouped insertion,
selection, an in-memory clipboard and capacity-preflighted literal replacement.
The UTF-8 cursor model is shared by prompts and source editing. No unbounded
containers or second source buffer are introduced for undo.

Native multi-curve sampling reuses the scoped DB48X evaluator in X-only mode,
leaving stored Y available. The temporary sample buffer is freed on return.
Graph scenes use a fixed-size, explicit little-endian versioned layout with
checksum, validated UTF-8 formula fields, finite ordered bounds, camera and
angle limits. Decode assigns only after validation; SD transactions reuse the
existing checked temporary/backup promotion. Native scene files are separate
from unchanged upstream calculator session formats.

## Port 7 shared plot backend

`plot_api.h` is the C boundary: a fixed request struct plus a sample callback.
`plot_engine.h` owns the production 300-point/curve and 17x17 surface sampling,
finite-value filtering, surface scaling, bounds and cancellation checks.
`desktop.cc` retains the native 2D presentation/controls and 3D wireframe view,
both accepting that request. DB48X adapters keep rooted scoped evaluators alive;
all calculator Graphs menu function paths use the native presentation.

Lua stores registry references in its bounded state and keeps a small native
request in the session context. Before entering C++, it reserves Lua stack space.
Each native-to-Lua sample calls lua_pcall in C, catches and copies any error,
and returns normally to C++. Only after the native renderer and its stack
objects return can plot.show raise a Lua error. Mutating/showing plots from
inside a sample is forbidden. Lua closures use their own state and do not
serialize as calculator scene formulas. The numerical source remains explicit:
DB48X expressions retain DB48X evaluation; Lua callbacks use Lua numbers.

## Port 8 numerical analysis and formula UI

Analysis samples through the production plot_source callback. Lua calls remain
protected in C; numeric statuses propagate back through the native view before
Lua raises any callback error. No Lua longjmp crosses the native analysis stack.
Root bracketing requires a small residual as well as interval convergence, so a
pole or finite jump is not accepted as a zero. A scan/local refinement may miss
features; errors/tolerances are estimates rather than formal proofs. Min/max
returns a sampled/refined interval extremum, including endpoints.

Integration is iterative adaptive Simpson with minimum refinement, capped work
and a callback/poll check before each evaluation. Slopes use Richardson-centred
samples with one-sided consistency and shrinking steps. Fixed local arrays avoid
unbounded recursive stack growth. Analysis overlays are transient and cleared
when the active curve/window changes; graph scenes retain their format.

`calculus.h` calls DB48X derivative/primitive methods with GC roots and restores
stack depth and scoped settings. Symbolic definite integration substitutes exact
bounds into a supported primitive and evaluates their difference. Numerical
constants/HFP/results are scoped off for symbolic work; normal calculator state
is restored. Unsupported primitives report the engine error. Source conversion
uses as_text(true), not display rendering, and validates length/UTF-8 before
replacing a formula. Formula-editor cancel restores the original buffer.


Port 9 extends the native view with a sampled crosshair, cached live numerical
tangent and transient visual interval selection (pico/plot_explore.h). The
analysis evaluator integrates abs(first-second) for between-curve area; both
curves share the existing global evaluation budget and cancellation/error path.
No changes to scene format or application flash writes are introduced.


Port 10 adds a bounded 1564-byte versioned workbench format embedding the
unchanged 760-byte graph-scene payload and a checksum. Native menu paging and
parameter-mode controls extend the existing desktop view; Lua's API is unchanged.
Parameter sampling and solver trials run in a registered temporary child of the
current directory, then remove it. Desktop resource transactions now share the
same reusable save/read implementation tested against actual FatFS failures.


Port 11 extends plot_source with an optional bounded XY scatter overlay. Fitted
functions use the existing callback sampler and analysis; residuals use a zero
baseline with transformed observations. CSV imports stage a candidate and commit
only after full validation. SD card reads use one checked single-sector command
per block to avoid the old malformed CMD18 token/CRC handling.


Ports12/13 bind named inputs and sequential Rn results within the existing scoped
child-directory mechanism, but restore caller arithmetic/function settings while
calculating. Returned exact objects are cloned and GC-rooted before child purge;
a root alone does not preserve an alias into deleted global directory storage.
Document codecs use explicit little-endian fields/checksums and validated staging;
notebook output previews are never serialized or consumed as future input.


Port 14 extends plot_source with an optional native XY path callback/context and
path scene. Existing C/Lua pico_plot_request and sample APIs stay unchanged;
path sampling is an additional C++ entry point with the same 1/0/-1/-2 finite/
gap/failure/interruption contract. Callback state and scene buffers outlive the
native modal view. The function-plot branch remains the same x-column geometry;
path geometry normalizes both coordinates. Cartesian analysis is gated away
from path sources, and T-tangent slopes derive from dY/dT divided by dX/dT.
A new optional calculus variable defaults to X; path editor transformations use
T. Parametric bindings use the same temporary child-directory isolation as the
workbench. All persistent resources remain under /db48x and all writes use SD.

Port 15 adds a bounded first-order RK4 step-doubling solver and ODE scene format.
Its 300 solution samples and 315 field slopes reuse the existing 900-double
plot cache. Native graph sampling dispatches through an optional ODE context;
F2 reports numerical status rather than applying Cartesian root/area analysis.
Unit-aware forms use upstream unit objects/conversion and clone captured axis
units before purging temporary bindings. V1 686-byte forms remain readable;
V2 is 910 bytes. SVG export streams the renderer's line geometry to a checked
FatFS sink; CSV streams cached values. No additional framebuffer, application
flash writes or unbounded heap allocation is introduced. SVG is a vector
representation, not a pixel-perfect screenshot.


Port 16 replaces upstream variable-sized C-stack scratch with fallible scoped
heap arrays, preserving GC-root construction/destruction and error returns.
The checked transaction layer stages the old primary as .rollback and delays
backup rotation until after promotion; validated loaders establish conservative
fingerprint-based rotation eligibility. SD command retries share an elapsed
initialization deadline. Lua's streaming reader is protected inside C, so I/O
errors do not longjmp across C++ resource destructors. Editor heap buffers are
released before Lua compilation and recreated from the saved file afterwards.
Explicit SDK-compatible nothrow allocation overloads avoid unused exception
runtime support. See reports/PORT16-REMEDIATION.md for invariants and limits.


## Port 17 network and conversion boundaries

Conversions remain in the platform layer and reuse `unit::convert` with scoped
calculator contexts. Currency decimal rates have their own strict versioned
codec and CSV parser; they are not installed as global unit definitions.
Currency updates validate all 29 requested quotes plus EUR before replacing RAM
or saving the cache. Existing session/workbench/scene formats are unchanged.

Wi-Fi is part of the colour firmware. Static packet pools live in previously
unallocated internal SRAM; TLS/session allocations use the existing bounded
heap. A generation token prevents delayed DNS callbacks from dereferencing
cancelled sessions. TCP callbacks detach before abort, TLS objects are freed,
and CYW43 deinitializes before returning to calculator UI. Stored variables,
calculator arena, framebuffer, stack and heap reservations are not repurposed.
Confirmed UTC is scoped to certificate verification; it does not write RTC or
application flash. Credentials are saved only by explicit UI action on SD.


## Port 17.3 time boundaries

UTC=0 in the network API selects automatic SNTP; nonzero is an explicit manual
current-UTC override. A dated API selects historical observations independently.
NTP validates origin cookie, endpoint, bounded packet fields and era conversion;
it is not authenticated. Failed time sync never disables TLS verification or
falls back to the unset boot clock. DNS lookup tokens advance per lookup, so late
NTP-server resolution cannot overwrite a subsequent provider resolution.

On return, accepted UTC updates the calculator's monotonic wall-clock anchor,
even if the later HTTPS/data/save phase fails. Display timezone is independent:
Toronto by default, named presets or a fixed UTC offset, saved separately on SD.
Legacy RTC writes retain their local-time semantics and clear the network anchor.
Rate-date requests never alter the current-time source. Clock time remains volatile.
