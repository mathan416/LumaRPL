// Non-blocking adapter: upstream callers retain control of acknowledgement/waits.
void legacy_message(const char *title,const char *const *lines,unsigned count,bool error)
{
    pico_ui::ScreenScope screen_scope(error?"legacy.error":"legacy.message");
    page(title,pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Back"}});action_bar({{nullptr,nullptr,nullptr,nullptr,error?"Dismiss":"Back"}});
    char text[1024];size_t used=0;
    for(unsigned i=0;i<count&&used<sizeof text-4;++i)if(lines[i])
    {
        if(used)text[used++]='\n';size_t n=strlen(lines[i]);bool truncated=n>=sizeof text-used;
        n=std::min(n,sizeof text-used-1);while(n&&!pico_text_editor::valid_utf8(lines[i],n))--n;
        memcpy(text+used,lines[i],n);used+=n;if(truncated&&used>=3){used-=3;memcpy(text+used,"...",3);used+=3;}
    }
    text[used]=0;
    pico_information_text::draw(Screen,native_compact_font(),text,used,0,8,45,304,16,14,0,pico_ui::text_colour());
    flush_display();
}
