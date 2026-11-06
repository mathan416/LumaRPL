#pragma once
#include <stdint.h>
// Synthetic event ranges are disjoint from DB48X's 1..99 key identifiers.
enum { PICO_TEXT=0x10000, PICO_SAVE=0x20001, PICO_LOAD,
       PICO_EXIT, PICO_LEFT, PICO_RIGHT, PICO_SETUP, PICO_PROGRAMS, PICO_GRAPH, PICO_WORKBENCH, PICO_DATA, PICO_FORMS, PICO_NOTEBOOK, PICO_CONVERSIONS, PICO_REFERENCE_FIND, PICO_CATALOG, PICO_RPL_SETTINGS, PICO_BASES, PICO_MATH_MENU };
static inline int pico_translate_key(int code)
{
    switch(code)
    {
    case 15:return PICO_MATH_MENU; // Ctrl+O: existing Math menu
    case 1:return PICO_CATALOG; // Ctrl+A: command catalogue
    case 25:return PICO_BASES; // Ctrl+Y: bases (source editors retain Redo)
    case 18:return PICO_RPL_SETTINGS; // Ctrl+R: Advanced RPL settings
    case 0x81:return 38; case 0x82:return 39; case 0x83:return 40;
    case 0x84:return 41; case 0x85:return 42; case 0x86:return 43;
    case 10:case 13:return 13;
    case 8:case 127:return 17;
    case 27:case 0xb1:case 0xd0:case 0x91:return 33;
    case 0xb5:return 18; case 0xb6:return 23;
    case 0xb4:return PICO_LEFT;case 0xb7:return PICO_RIGHT;
    case 19:return PICO_SAVE;case 12:return PICO_LOAD;case 17:return PICO_EXIT;
    case 9:return 28;
    case 11:return PICO_REFERENCE_FIND; // Ctrl+K: command-reference lookup
    case 21:return PICO_CONVERSIONS; // Ctrl+U
    case 14:return PICO_NOTEBOOK;
    case 6:return PICO_FORMS;
    case 4:return PICO_DATA;
    case 23:return PICO_WORKBENCH;
    case 20:return PICO_SETUP;case 16:return PICO_PROGRAMS;case 7:return PICO_GRAPH;
    case 2:return PICO_TEXT+0xab; // Ctrl+B: begin RPL program
    case 5:return PICO_TEXT+0xbb; // Ctrl+E: end RPL program
    default:return code>=32 && code<127 ? PICO_TEXT+code : -1;
    }
}
static inline uint8_t pico_mono_pixel(const uint8_t *frame,int x,int y)
{
    int source_x=319-x;
    return frame[y*40+source_x/8] & (1u<<(source_x%8)) ? 255:0;
}

static inline uint32_t pico_rgb565_to_rgb(uint16_t v)
{
    uint8_t r=(v>>11)&31,g=(v>>5)&63,b=v&31;
    return ((uint32_t)((r<<3)|(r>>2))<<16)|((uint32_t)((g<<2)|(g>>4))<<8)|((b<<3)|(b>>2));
}
static inline void pico_rgb_pixel(const uint8_t *frame,int x,int y,uint8_t rgb[3])
{
#ifdef PICOCALC_RGB332
    unsigned q=frame[y*320+319-x];rgb[0]=((q>>5)*255+3)/7;rgb[1]=(((q>>2)&7)*255+3)/7;rgb[2]=(q&3)*85;return;
#endif
    int offset=2*(y*320+319-x);
    uint16_t v=frame[offset] | ((uint16_t)frame[offset+1]<<8);uint32_t colour=pico_rgb565_to_rgb(v);
    rgb[0]=(colour>>16)&255;rgb[1]=(colour>>8)&255;rgb[2]=colour&255;
}
