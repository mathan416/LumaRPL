// Shared firmware/host UI implementation. GPL-3.0.
namespace source_layout {
constexpr int x=35,y=38,cell=7,line=14,columns=40,rows=16,caret_height=12;
inline font_p font(){return font_p(::PicoSourceFont_sparse_font_data);}
}
void editor_caret(size_t row,size_t col,size_t scroll)
{
    int x=source_layout::x+col*source_layout::cell,y=source_layout::y+(row-scroll)*source_layout::line;Screen.invert(x,y,x+1,y+source_layout::caret_height-1);
}
void editor_position(const pico_text_editor &editor,const pico_editor_tools &tools,bool hints)
{
    size_t line=1,column=1;
    for(size_t i=0;i<editor.cursor;i=editor.next(i)){if(editor.text[i]=='\n'){++line;column=1;}else ++column;}
    Screen.fill(0,266,151,285,pattern(Settings.Background()));
    char status[48];snprintf(status,sizeof status,"Ln %u  Col %u",unsigned(line),unsigned(column));
    ui_line(4,268,148,16,status,pico_ui::TextRole::status,pico_ui::text_colour());
    if(hints){Screen.fill(152,266,319,285,pattern(Settings.Background()));ui_line(160,268,156,16,tools.selected()?"Ctrl+O tools | selection":"Ctrl+O tools",pico_ui::TextRole::status,pico_ui::text_colour());}
}
bool move_editor_cursor(pico_text_editor &editor,size_t scroll,pico_editor_tools &tools,int key)
{
    if(!tools.ready()||tools.anchor!=pico_editor_tools::none)return false;
    pico_text_editor next=editor;
    if(key==0xb4)next.cursor=next.previous(next.cursor);
    else if(key==0xb7)next.cursor=next.next(next.cursor);
    else if(key==0xb5)next.vertical(-1,source_layout::columns);
    else if(key==0xb6)next.vertical(1,source_layout::columns);
    else return false;
    tools.break_group();if(next.cursor==editor.cursor)return true;
    size_t old_row,old_col,row,col;editor.position(editor.cursor,old_row,old_col,source_layout::columns);next.position(next.cursor,row,col,source_layout::columns);
    if(row<scroll||row>=scroll+source_layout::rows||old_row<scroll||old_row>=scroll+source_layout::rows)return false;
    editor_caret(old_row,old_col,scroll);editor.cursor=next.cursor;editor_caret(row,col,scroll);
    editor_position(editor,tools,false);
    mark_dirty(source_layout::x+old_col*source_layout::cell,source_layout::y+(old_row-scroll)*source_layout::line,source_layout::x+1+old_col*source_layout::cell,source_layout::y+source_layout::caret_height-1+(old_row-scroll)*source_layout::line);
    mark_dirty(source_layout::x+col*source_layout::cell,source_layout::y+(row-scroll)*source_layout::line,source_layout::x+1+col*source_layout::cell,source_layout::y+source_layout::caret_height-1+(row-scroll)*source_layout::line);
    mark_dirty(0,266,151,285);lcd_refresh();pico_ui_cursor_complete();return true;
}
void render_editor(pico_text_editor &editor,const char *name,size_t &scroll,source_language language,const pico_editor_tools &tools,bool chrome=true)
{
    char heading[80];snprintf(heading,sizeof heading,"%s%s",editor.dirty?"* ":"",name);
    if(chrome)page(heading,pico_ui::Actions{{"Save",language==source_language::assembly?"Build+run":"Save+run",nullptr,"Tools","Back"}});else Screen.fill(0,28,319,265,pattern(Settings.Background()));
    size_t row,col;editor.position(editor.cursor,row,col,source_layout::columns);
    if(row<scroll)scroll=row;if(row>=scroll+source_layout::rows)scroll=row-(source_layout::rows-1);
    pico_source_style style(language==source_language::lua?pico_source_style::lua:language==source_language::assembly?pico_source_style::assembly:pico_source_style::rpl);
    size_t r=0,c=0,line=1,column=1,word_end=0;bool word_keyword=false;
    for(size_t i=0;i<=editor.length;i=editor.next(i))
    {
        if(r>=scroll+source_layout::rows)break;
        if(c==0 && r>=scroll)
        {char number[12];snprintf(number,sizeof number,"%u",unsigned(line));Screen.text(2,40+(r-scroll)*source_layout::line,utf8(number),native_compact_font(),pico_ui::readable(pattern(100,110,130),pattern(Settings.Background())));}
        if(i==editor.length)break;
        unsigned char ch=static_cast<unsigned char>(editor.text[i]);
        auto token=style.next(ch,i+1<editor.length?static_cast<unsigned char>(editor.text[i+1]):0);
        if(ch=='\n'){++r;c=0;++line;column=1;continue;}
        if(language==source_language::lua && token==pico_source_style::plain)
        {
            if(i>=word_end)
            {word_end=i;while(word_end<editor.length&&(isalnum(static_cast<unsigned char>(editor.text[word_end]))||editor.text[word_end]=='_'))++word_end;word_keyword=pico_source_style::lua_keyword(editor.text+i,word_end-i);}
            if(i<word_end&&word_keyword)token=pico_source_style::keyword;
        }
        if(r>=scroll)
        {
            char glyph[5]={};size_t n=editor.next(i)-i;memcpy(glyph,editor.text+i,n);
            bool light=pico_ui::luminance(pattern(Settings.Background()))>17900000;
            pattern colour=token==pico_source_style::comment?(light?pattern(24,95,32):pattern(100,180,100)):token==pico_source_style::string?(light?pattern(135,60,0):pattern(240,180,70)):token==pico_source_style::number?(light?pattern(0,80,125):pattern(60,200,220)):token==pico_source_style::keyword?(light?pattern(100,40,165):pattern(190,150,240)):token==pico_source_style::delimiter?(light?pattern(145,35,25):pattern(245,135,100)):pattern(Settings.EditorForeground());
            if(tools.selected()&&i>=tools.first()&&i<tools.last())Screen.fill(source_layout::x+c*source_layout::cell,source_layout::y+(r-scroll)*source_layout::line,source_layout::x+source_layout::cell-1+c*source_layout::cell,source_layout::y+source_layout::line-1+(r-scroll)*source_layout::line,pattern(35,75,120));
            colour=tools.selected()&&i>=tools.first()&&i<tools.last()?pattern(255,255,255):pico_ui::readable(colour,pattern(Settings.Background()));
            Screen.text(source_layout::x+c*source_layout::cell,source_layout::y+(r-scroll)*source_layout::line,utf8(glyph),source_layout::font(),colour);
        }
        ++column;if(++c==source_layout::columns){++r;c=0;}
    }
    editor_caret(row,col,scroll);editor_position(editor,tools,true);
}
