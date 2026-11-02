// Shared firmware/host UI implementation. GPL-3.0.
void lua_output_view(const char *title,const char *text,size_t length)
{
    pico_ui::ScreenScope screen_scope("lua.output");
    bool aligned=false;
    for(;;)
    {
        int action=view_text(title,text,length,aligned?pico_ui::TextRole::table:pico_ui::TextRole::prose,pico_ui::Actions{{nullptr,nullptr,aligned?"Normal":"Columns",nullptr,"Back"}});
        if(!action)return;if(action==3)aligned=!aligned;
    }
}
