#pragma once
#include "native_layout.h"
inline void run_layout_engine_tests()
{
    using namespace pico_native_layout;using canvas=surface;using ink=canvas::pattern;
    font_p small=font_p(PicoHeaderFont_sparse_font_data);char fitted[320];
    surface_check(small->height()<=compact_height,"native compact font fits status rows");
    printf("Compact font cell width: %d pixels\n",compact_cell_width(small));
    surface_check(compact_cell_width(small)<=10,"compact fixed cells preserve the previous 30-column width");
    surface_check(information_top+(information_rows-1)*information_row_step+small->height()<=footer_top,"all fifteen compact rows remain above footer");
    const char *cases[]={"A=1.234567e+99 step 1.2345e+99 Enter edit Esc trace","X=-1.234567e+99 Y=-1.234567e+99","dY/dX=-1.234567e+99 | F4 Pan +/- Zoom","F1 Functions F2 Templates F3 Check","F1 Save F2 Build+Run F5 Close","F2 Export | Arrows rotate +/- zoom","Page 100/100 | Left/Right pages","Result(m/s^2) vs DISTANCE(cm)","Differential equation: αβγδεζηθι🙂","A saved file with a very long name that needs a visible ellipsis.workbench"};
    for(auto text:cases){font_p chosen=fit(fitted,sizeof fitted,text,304,16,HelpFont,small);surface_check(chosen->width(utf8(fitted))<=304&&chosen->height()<=16&&pico_text_editor::valid_utf8(fitted,strlen(fitted)),"bounded native status/menu text");}
    surface_check(graph_formula+16<=graph_values&&graph_values+16<=graph_footer_top&&graph_status>=graph_footer_top&&graph_status+13<=graph_controls&&graph_controls+13<=height,"graph status/footer regions never overlap");
    static blitter::pixword pixels[320*320*canvas::BPP/32];canvas screen(pixels,320,320);screen.fill(ink(0,0,0));
    draw(screen,5,graph_status,310,13,cases[0],small,small,ink(255,255,255));
    for(int y=0;y<320;++y)for(int x=0;x<320;++x){auto c=screen.pixel_color(x,y);if(c.red()||c.green()||c.blue())surface_check(x>=5&&x<315&&y>=graph_status&&y<graph_status+13,"text raster stays inside assigned row");}
    auto line=[&](int x,int y,int w,int h,const char *text,font_p font=HelpFont){draw(screen,x,y,w,h,text,font,small,ink(255,255,255));};
    auto save=[&](const char *name){char path[160];snprintf(path,sizeof path,"../reports/%s.ppm",name);FILE *f=fopen(path,"wb");surface_check(f,"open native layout preview");fprintf(f,"P6\n320 320\n255\n");for(int y=0;y<320;++y)for(int x=0;x<320;++x){auto c=screen.pixel_color(x,y);unsigned char rgb[]={c.red(),c.green(),c.blue()};fwrite(rgb,1,3,f);}fclose(f);};
    // Same text helper, fonts and region coordinates as the firmware.
    screen.fill(ink(15,22,32));screen.fill(0,0,319,27,ink(24,44,80));screen.fill(0,graph_footer_top,319,319,ink(24,44,80));line(8,3,304,23,"Parameter plot: tangent");
    for(int i=0;i<=10;++i)screen.line(10+i*30,35,10+i*30,244,1,ink(70,85,100));for(int j=0;j<=8;++j)screen.line(10,35+j*26,309,35+j*26,1,ink(70,85,100));
    for(int i=1;i<300;++i)screen.line(9+i,140-int(70*sin((i-1)*.025)),10+i,140-int(70*sin(i*.025)),2,ink(60,170,240));
    line(5,graph_formula,310,16,"Y1: A*SIN(B*X+C)",small);line(5,graph_values,310,16,cases[1],small);line(5,graph_status,310,13,cases[0],small);line(5,graph_controls,310,13,"F1 Table F2 Analyse F3 Actions",small);save("port151-layout-graph");
    screen.fill(ink(15,22,32));screen.fill(0,0,319,27,ink(24,44,80));screen.fill(0,289,319,319,ink(24,44,80));line(8,3,304,23,"Differential equations");
    const char *menu[]={"Plot solution / direction field","Edit dy/dX = f(X,Y)","Initial condition X0, Y0","View window","Examples","Save / load equations","Help","Back"};
    for(int i=0;i<8;++i){if(i==0)screen.fill(0,37+i*27,319,58+i*27,ink(30,80,150));line(10,37+i*27,305,22,menu[i]);}line(6,297,308,18,"Arrows/Enter | F1-F5 | Esc: back",small);save("port151-layout-menu");
    screen.fill(ink(15,22,32));screen.fill(0,0,319,27,ink(24,44,80));screen.fill(0,footer_top,319,319,ink(24,44,80));line(8,3,304,23,"Rates not updated");
    const char *diagnostic[]={"Wi-Fi association failed after retries;", "see radio event codes", "", "Radio event: 7", "Event status: 1 | Reason: 17", "Time: NTP not completed", "Wi-Fi status: -1", "Join time: 3027 ms | Requests: 3", "TLS error: 0", "TLS peak: 0 bytes", "Session: 4344 bytes", "Radio is off."};
    for(unsigned i=0;i<sizeof diagnostic/sizeof *diagnostic;++i)
    {
        surface_check(small->width(utf8(diagnostic[i]))<=308,"diagnostic text fits compact body width");
        surface_check(37+i*16+small->height()<footer_top,"diagnostic rows stay above footer");
        line(6,37+i*16,308,16,diagnostic[i],small);
    }
    line(6,footer_text,308,18,"Up/Down scroll | Esc back",small);save("port182-layout-diagnostics");
    screen.fill(ink(15,22,32));screen.fill(0,0,319,27,ink(24,44,80));screen.fill(0,footer_top,319,319,ink(24,44,80));line(8,3,304,23,"Memory / PSRAM");
    const char *memory[]={"PSRAM: available / verified", "Capacity: 8192 KiB", "Backing slots reserved: 8128 KiB", "Parkable document payload: 8392 bytes", "Editor undo working structure: 572 bytes", "", "Internal SRAM budgets:", "Calculator arena: 208 KiB", "Heap: 96 KiB (Lua limit: 48 KiB)", "Stack reservation: 32 KiB", "256-colour framebuffer: 100 KiB", "Shared editor / plot buffer: 16 KiB", "Largest workspace request: 7200 bytes", "", "Inactive documents use checked PSRAM."};
    for(unsigned i=0;i<sizeof memory/sizeof *memory;++i)
    {
        surface_check(small->width(utf8(memory[i]))<=308,"memory diagnostics fit compact body width");
        line(6,information_top+i*information_row_step,308,information_row_step,memory[i],small);
    }
    line(6,footer_text,308,18,"Up/Down scroll | Esc back",small);save("port183-layout-memory");
    screen.fill(ink(15,22,32));screen.fill(0,0,319,27,ink(24,44,80));screen.fill(0,footer_top,319,319,ink(24,44,80));line(8,3,304,23,"Battery status");
    const char *battery[]={"Controller level: 87%", "Charging: no", "Last reply: 0x0057", "Valid sample age: 0 s", "Keyboard BIOS: 1.2 (0x12)", "Write 1/1 | Read 2/2", "Valid battery response", "500 ms timeout | 16 ms pause", "-1: bus error  -2: timeout"};
    for(unsigned i=0;i<sizeof battery/sizeof *battery;++i)line(8,40+i*27,304,16,battery[i],small);
    line(6,footer_text,308,18,"F1 refresh | Esc back",small);save("port183-layout-battery");
    // Fixed-cell mode keeps even narrow glyphs and spaces at the same advance.
    screen.fill(ink(15,22,32));screen.fill(0,0,319,27,ink(24,44,80));screen.fill(0,footer_top,319,319,ink(24,44,80));line(8,3,304,23,"Lua output / matrix");
    const char *table[]={"   X       Y", "   1      10", "  12     100", " 123    1000", "", "[[ 1  2 ]", " [ 3  4 ]]"};
    int cell=compact_cell_width(small);
    for(unsigned row=0;row<sizeof table/sizeof *table;++row)
        for(unsigned col=0;table[row][col];++col)
        {
            char glyph[]={table[row][col],0};
            line(6+col*cell,information_top+row*information_row_step,cell,information_row_step,glyph,small);
        }
    line(6,footer_text,308,18,"Up/Down scroll | Esc back",small);save("port183-layout-output");
    puts("PASS: native text font metrics, UTF-8 elision, raster clipping and graph/footer geometry; representative 320x320 previews");
}
