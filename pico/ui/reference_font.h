#pragma once
#include "font.h"
extern const unsigned char PicoReferenceFont_sparse_font_data[];
namespace pico_ui {
inline font_p reference_font(){return font_p(::PicoReferenceFont_sparse_font_data);}
}
