// Shared firmware/host UI implementation. GPL-3.0.
font_p native_compact_font(){return font_p(::PicoHeaderFont_sparse_font_data);}
void ui_line(int x,int y,int width,int height,const char *text,pico_ui::TextRole role,pattern foreground)
{
    if(width<=0||height<=0)return;auto background=Screen.pixel_color(std::max(0,std::min(319,x)),std::max(0,std::min(319,y)));foreground=pico_ui::readable(foreground,pattern(background));
    pico_native_layout::draw(Screen,x,y,width,height,text,pico_ui::font(role),native_compact_font(),foreground);
}
void label(int x,int y,const char *text,bool selected=false)
{
    pattern fg=selected?pattern(Settings.SelectionForeground()):pico_ui::text_colour();
    pattern bg=selected?pattern(Settings.SelectionBackground()):pattern(Settings.Background());
    int height=std::min(22,pico_native_layout::footer_top-y);if(height<=0)return;
    if(selected)Screen.fill(0,y,319,y+height-1,bg);
    ui_line(x,y,320-x-5,height,text,pico_ui::TextRole::menu,fg);
}
// Non-selectable information stays compact; label() remains the menu renderer.
void info_label(int x,int y,const char *text)
{
    int height=std::min(16,pico_native_layout::footer_top-y);if(height<=0)return;
    ui_line(x,y,320-x-5,height,text,pico_ui::TextRole::status,pico_ui::text_colour());
}
void action_bar(const pico_ui::Actions &actions)
{
    Screen.fill(0,pico_native_layout::footer_top,319,319,pattern(Settings.HeaderBackground()));
    for(int i=0;i<5;++i)if(actions.label[i]&&*actions.label[i])
    {
        char key[]={ 'F',char('1'+i),0};
        ui_line(i*64+4,290,56,13,key,pico_ui::TextRole::status,pattern(255,255,255));
        ui_line(i*64+4,304,56,14,actions.label[i],pico_ui::TextRole::status,pattern(255,255,255));
    }
}
void page(const char *title,pico_ui::Actions actions={{nullptr,nullptr,nullptr,nullptr,"Back"}})
{
    if(pico_ui::active_screen!=pico_ui::visible_screen)key_pop_all();
    pico_ui::seen(title);++pico_ui::page_generation;Screen.clip(Screen.area());Screen.fill(pattern(Settings.Background()));Screen.fill(0,0,319,27,pattern(Settings.HeaderBackground()));
    ui_line(8,3,304,23,title,pico_ui::TextRole::title,pattern(255,255,255));action_bar(actions);
}

// The visible row numbers mean the same thing in all native file lists.
void file_row_key(int &key,int &selected,int total)
{int row=pico_native_menu::shortcut(key,selected,total);if(row>=0){selected=row;key=13;}}
void file_row_label(int row,const char *name,bool selected)
{int y=38+row*28;label(24,y,name,selected);char number[]={char('1'+row),0};ui_line(7,y+3,14,16,number,pico_ui::TextRole::status,selected?pattern::white:pico_ui::text_colour());}

void list_page_hint(int selected,int total,bool buttons=false)
{
    if(total<=8)return;
    int offset=pico_native_menu::offset(selected);char hint[96];
    snprintf(hint,sizeof hint,"%d-%d of %d  |  Page %d/%d",offset+1,std::min(offset+8,total),total,offset/8+1,(total+7)/8);
    if(buttons){info_label(8,253,hint);info_label(8,270,"F2 / Left: previous   F3 / Right: next");}
    else{snprintf(hint,sizeof hint,"%d-%d of %d | Page %d/%d | Left / Right",offset+1,std::min(offset+8,total),total,offset/8+1,(total+7)/8);info_label(8,270,hint);}
}

// Shared body-only redraw for native readers, forms and dialogs.
bool content_page(const char *title,pico_ui::Actions actions,unsigned &frame)
{bool chrome=frame!=pico_ui::page_generation;if(chrome)page(title,actions);else{Screen.clip(Screen.area());Screen.fill(0,28,319,288,pattern(Settings.Background()));}return chrome;}
void content_refresh(bool chrome,unsigned &frame)
{if(chrome)flush_display();else{mark_dirty(0,28,319,288);lcd_refresh();}frame=pico_ui::page_generation;}
