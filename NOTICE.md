# Source and license notices

LumaRPL calculator design direction and device testing: Iain Bennett.
The About / credits screen acknowledges the components below; full upstream
licenses, authorship and copyright notices remain included in the release.

LumaRPL incorporates DB48X by Christophe de Dinechin and its
contributors under its upstream LGPL-3.0 terms, plus ClockworkPi-derived UF2
Loader drivers under GPL-3.0. The combined port is distributed under GPL-3.0;
individual upstream components retain their notices and applicable licenses.
No HP or TI proprietary ROM is included.

- DB48X: https://github.com/c3d/db48x — `db48x/LICENSE.txt`, `LEGAL-NOTICE.md`
  and `AUTHORS`; copied platform files retain their original headers.
- UF2 Loader / PicoCalc drivers: https://github.com/pelrun/uf2loader, v2.4.1.
  Copyrights and GPL terms are in its source and `LICENSE`.
- FatFS R0.16: Copyright 2025 ChaN; permissive notice retained in all sources.
  The writable FatFS copy came from loader commit
  `5c44a4b64749062b0200507ceeff3ef2b475e288`.
- Raspberry Pi Pico SDK 2.2.0: its BSD-3-Clause license and third-party component
  notices are preserved in the corresponding source; SDK math/runtime components
  can carry additional notices. TinyUSB is fetched at the SDK's pinned revision
  but USB is disabled in this application.
- SwissMicros DMCP interface declarations: BSD-3-Clause notice preserved in
  `pico/dmcp.h`; no SwissMicros ROM function table is called.
- Newlib, libstdc++ and GCC runtime retain their upstream licenses/exceptions;
  the complete Arm embedded toolchain used is identified in BUILDING.md.

Local hardware backend, build tooling and tests are provided under GPL-3.0.
See LICENSE for the full GPL text. The source archive and immutable lock file
are supplied so the linked firmware can be inspected, rebuilt and modified.

- Lua 5.4.9: Copyright 1994-2026 Lua.org, PUC-Rio, under the MIT license.
  Official archive SHA-256 and URL are pinned in SOURCES.lock.json; original
  source and doc/readme.html license are included. Only selected libraries are
  opened by the host application. Lua is linked from C, separately from DB48X.
- Timendus CHIP-8 test suite: source/revision/license retained under dependencies/
  in the source archive for validation. ROM-specific attribution is retained.
  These test ROMs are not bundled into the ordinary SD game browser.
- CHIP-8 VM, native assembler, original MOVE demo, Lua/RPL adapters and editors
  are local GPL-3.0 port code. No proprietary game ROM or HP interpreter binary
  is distributed. Octo is an external development option and is not linked.


Networking uses the SDK-pinned CYW43 driver under its Raspberry Pi
semiconductor license (`licenses/CYW43-RP.txt`), lwIP under its BSD notice
(`licenses/LWIP.txt`), and separately pinned Mbed TLS 3.6.7 under its provided
dual-license terms (`licenses/MBEDTLS.txt`). Original source notices remain in
all dependency trees; the Wi-Fi firmware resources are supplied with the driver.
The public GTS Root R4 certificate comes from
https://pki.goog/repo/certs/gtsr4.pem; its DER fingerprint is recorded in
`pico/network/ca_root.h`. Frankfurter dated reference
observations are cached with provider attribution; https://frankfurter.dev/.
No Wi-Fi passwords or user credential files are included in the release bundle.

The platform includes original GPL-3.0 PicoCalc serial-PSRAM storage and PIO transport, plus
RGB332 screen rendering. No external allocator or vendor PSRAM source is linked.
The onboard wiring follows ClockworkPi's published mainboard design; protocol
constraints were checked against Espressif's ESP-PSRAM64/64H datasheet. Existing
DB48X, Pico SDK, CYW43, lwIP, Mbed TLS, Lua, display-driver and font notices remain
applicable and are distributed in the source and SD license bundle.

Compact source font: Pixel Operator Mono by Jayvee Enaguas (GrandChaos9000),
(C) 2009–2015 The Grandoplex Project, under SIL OFL 1.1. The derived
PicoSourceFont uses fixed 7x12 cells and resizes supplemental FogSans glyphs.
FogSans font software copyright 2012 Intel Corporation, Apache-2.0; upstream
FogSans-ddd modifications and source remain included. Original font notices
and full license texts are in licenses/SOURCE-FONT-OFL.txt and
licenses/SOURCE-FONT-APACHE.txt. Font data retains its font-license terms;
the generation and editor code are GPL-3.0.


## Additional platform and development acknowledgements

ClockworkPi's PicoCalc hardware and published design are acknowledged separately
from the copied/adapted driver source. See https://github.com/clockworkpi/PicoCalc.
CMSIS (Arm), Pico printf (Marco Paland and contributors), picotool (Raspberry Pi),
and the GNU toolchain/runtime notices are retained under licenses/ and the pinned
source trees. Qt and QEMU are development/validation tools, not linked firmware.
TinyUSB is a fetched SDK build dependency; this application disables USB.

The on-device credit summary now includes Pixel Operator Mono and FogSans authors.
Generated font data retains its underlying font licenses; a generator's GPL notice
does not replace those font licenses.

## Manuals and cover artwork

The User Guide and Programming Guide apply to the documented LumaRPL release and
use original explanatory text, code-checked examples and production UI captures.
Hewlett-Packard's HP 48G guides are a documentation-design inspiration and further
reading, not a software dependency or source of copied manual prose. LumaRPL is an
independent calculator implementation based on DB48X.

The cover photograph was supplied by Iain Bennett for these manuals. Software
license grants do not assign ownership of that photograph. The PDF typography
uses embedded DejaVu fonts, with original font notices reproduced in each PDF and
in docs/manuals/PDF-FONT-NOTICES.txt. ReportLab, Poppler, Pillow, pypdf and pdfplumber are documentation build,
image-preparation and PDF-validation tools only. The full component inventory for the manuals is in
docs/manuals/credits.json; this is an acknowledgement summary, not a replacement
for individual upstream license texts.
