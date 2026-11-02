#pragma once
#ifdef PICOCALC_RGB332
using p8=blitter::pattern<blitter::RGB_8BPP>;
const p8 p8::black(0,0,0),p8::white(255,255,255),p8::invert(~0ULL);
#endif
