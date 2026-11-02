// LumaRPL UI roles. GPL-3.0. Fonts and mathematical rendering retain their licenses.
#pragma once
#include "native_layout.h"
#include "settings.h"
#include <algorithm>
extern const unsigned char PicoHeaderFont_sparse_font_data[];
namespace pico_ui {
enum class TextRole {title,menu,prose,status,code,table,result,plot_label};
inline font_p compact(){return font_p(::PicoHeaderFont_sparse_font_data);}
inline font_p font(TextRole role)
{
    return role==TextRole::title||role==TextRole::menu?HelpFont:role==TextRole::result?Settings.result_font():role==TextRole::code?LibMonoFont10x17:compact();
}
inline unsigned page_generation=0;
struct MenuState {int selected=0;};
// Bounded focus cache: keys are immutable call-site addresses in flash.
struct Focus {const char *site=nullptr;MenuState state;};
inline MenuState &focus(const char *site)
{
    static Focus entries[96];static unsigned next=0;
    for(auto &entry:entries)if(entry.site==site)return entry.state;
    auto &entry=entries[next++%96];entry.site=site;entry.state.selected=0;return entry.state;
}
struct Actions {const char *label[5];};
}

namespace pico_ui {
struct Revision {bool dirty=false;unsigned serial=0;void saved(){dirty=false;++serial;}};
template<class T>struct Mutation {
    T before;T &value;Revision &revision;unsigned serial;
    Mutation(T &v,Revision &r):value(v),revision(r),serial(r.serial){memcpy(&before,&v,sizeof(T));}
    ~Mutation(){if(serial==revision.serial&&memcmp(&before,&value,sizeof(T)))revision.dirty=true;}
};
}

namespace pico_ui {
// Relative luminance lookup at the actual RGB332 channel levels, scaled 1e8.
inline uint32_t luminance(pattern value)
{
    static constexpr uint32_t red_green[]={0,176,666,1529,2874,4678,7084,10000};
    static constexpr uint32_t blue[]={0,908,4020,10000};
    blitter::pattern<blitter::mode::RGB_8BPP> quantized(value);unsigned c=quantized.bits&255;
    return 2126*red_green[c>>5]+7152*red_green[(c>>2)&7]+722*blue[c&3];
}
inline bool contrast(pattern foreground,pattern background)
{uint32_t a=luminance(foreground),b=luminance(background);if(a<b)std::swap(a,b);return 100ull*(a+5000000u)>=455ull*(b+5000000u);}
inline pattern readable(pattern foreground,pattern background)
{return contrast(foreground,background)?foreground:luminance(background)>=17900000?pattern::black:pattern::white;}
// Native graph strokes keep their hue where possible; document colours stay intact.
inline bool plot_contrast(pattern foreground,pattern background)
{uint32_t a=luminance(foreground),b=luminance(background);if(a<b)std::swap(a,b);return 100ull*(a+5000000u)>=305ull*(b+5000000u);}
inline pattern plot_ink(pattern foreground,pattern background)
{
    if(plot_contrast(foreground,background))return foreground;
    blitter::color<blitter::RGB_16BPP> original(uint32_t(foreground.bits));
    int target=luminance(background)>=17900000?0:255;
    for(int step=1;step<=8;++step)
    {
        pattern adjusted((original.red()*(8-step)+target*step)/8,
                         (original.green()*(8-step)+target*step)/8,
                         (original.blue()*(8-step)+target*step)/8);
        if(plot_contrast(adjusted,background))return adjusted;
    }
    return readable(foreground,background);
}
inline pattern text_colour(){return readable(pattern(Settings.StackForeground()),pattern(Settings.Background()));}
}
