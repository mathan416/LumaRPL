# LumaRPL PDF manuals

These are maintained, original User and Programming guides for LumaRPL on an
official Pico 2 W / ClockworkPi PicoCalc, using the current release-candidate
calculator behaviour. HP's classic manuals inspired the tutorial/reference split;
HP prose, illustrations and page layouts were not copied.

## Sources

- user-guide.md: calculator operation and worked workflows.
- programming-guide.md: editor, RPL, Lua and CHIP-8 tutorials.
- docs/help/lua-reference.json and chip8-reference.json: the same reviewed language
  reference content used on device, included as book appendices.
- credits.json: roles, project references and attribution summary for both books.
- PDF-FONT-NOTICES.txt: original notices for embedded PDF typography.
- assets/: the supplied, EXIF-stripped photograph and labelled production UI renders.

## Build

Use Python with reportlab, Pillow and pypdf. PDF layout review additionally uses
pdfplumber and Poppler's pdftoppm. The tested package versions are recorded in
requirements-pdf.txt. Set LUMARPL_PDF_FONTS to an installed DejaVu font directory
containing DejaVuSans.ttf, DejaVuSans-Bold.ttf, DejaVuSans-Oblique.ttf and
DejaVuSansMono.ttf. The builder also discovers the Codex bundled fonts on this
workstation. Font licenses remain in this folder and in the PDFs.

From the repository root:

```sh
python3 tools/build_manuals.py
python3 tools/test_manual_examples.py
python3 tools/review_manuals.py
```

The example tests require the production simulator/controller build described
in BUILDING.md, including the help-console and Lua probe objects. They compile
against actual calculator/Lua/assembler/VM implementations. Lua device I/O is
substituted, native plot callbacks use the production sampler, and complete
CHIP-8 examples receive synthetic keys/timer ticks. This is host verification,
not a physical display/sound or device acceptance claim.

Outputs are output/pdf/LumaRPL-User-Guide.pdf and
output/pdf/LumaRPL-Programming-Guide.pdf, with source, example and layout reports.
The review tool renders every page to tmp/pdfs and builds a numbered contact-sheet
index. Inspect the latest renders after changing text; bounds checks alone do not
establish visual quality. Generated PDFs/renders stay out of Git.

## Credits and compatibility audit

The manuals acknowledge runtime components, hardware/driver origins, fonts and
build/validation tools separately. The on-device credits source now includes
Pixel Operator Mono and FogSans authors. NOTICE.md removes obsolete port/report
paths. Complete direct component notices are maintained in licenses/, and the
release packager copies them into /db48x/licenses/ through tools/license_bundle.py.

Two stale help statements were corrected at their generator source: decimal
integers convert directly with Bin/Oct/Dec/Hex, and CSV observations live under
/db48x/datasets/ rather than the RPL source directory. The calculus route also
uses its current menu names. These descriptions match existing calculator code.

Existing UF2/SD release archives and the mounted SD card are untouched. On-device
credit/help text changes take effect in a subsequent firmware build/deployment;
creating manuals does not silently replace an installed firmware.
