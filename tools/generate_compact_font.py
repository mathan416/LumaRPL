#!/usr/bin/env python3
"""Generate the compact face without cropping negative-bearing glyph bitmaps."""
from pathlib import Path
import subprocess,shutil
root=Path(__file__).resolve().parents[1];out=root/'build/compact-font';out.mkdir(exist_ok=True)
source=(root/'db48x/tools/ttf2font/ttf2font.cpp').read_text()
# The upstream dense-layout adjustment trims the right edge of sparse glyphs
# (notably 5) with negative left bearings. Shift them inside their cell instead.
assert source.count('colsGlyph += colsBeforeGlyph;')==1, 'Pinned generator has changed'
source=source.replace('colsGlyph += colsBeforeGlyph;','/* Preserve the complete sparse bitmap; clamp only its origin. */')
(out/'generator.cc').write_text(source)
flags=subprocess.check_output(['pkg-config','--cflags','--libs','freetype2'],text=True).split() if shutil.which('pkg-config') else ['-I/opt/homebrew/opt/freetype/include/freetype2','-L/opt/homebrew/opt/freetype/lib','-lfreetype']
subprocess.run(['clang++','-std=c++17','-I'+str(root/'db48x/src'),str(out/'generator.cc'),*flags,'-o',str(out/'ttf2font')],check=True)
subprocess.run([str(out/'ttf2font'),'-s','12','-S','80','-y','-2','PicoHeaderFont','db48x/fonts/FogSans-ddd.ttf','pico/HeaderFont.cc'],cwd=root,check=True)
