// Shared firmware/host UI implementation. GPL-3.0.
int view_text(const char *title,const char *body,size_t length,pico_ui::TextRole role,pico_ui::Actions actions)
{
    key_pop_all();
    size_t scroll=0;const bool fixed_cells=role==pico_ui::TextRole::table||role==pico_ui::TextRole::code;const size_t visible=14;
    const int cell=fixed_cells?pico_native_layout::compact_cell_width(native_compact_font()):0;
    unsigned frame=~0u;
    for(;;)
    {
        bool chrome=content_page(title,actions,frame);size_t total=0;
        total=pico_information_text::draw(Screen,native_compact_font(),body,length,scroll,6,pico_native_layout::information_top,308,pico_native_layout::information_row_step,visible,cell,pico_ui::text_colour());
        if(!length)info_label(8,55,"No printed output");
        if(total>=visible)info_label(8,272,"Up/Down: scroll | Left/Right: page");

        content_refresh(chrome,frame);
        for(;;)
        {
        int k=read_key_raw();size_t previous=scroll;
        if(k>=0x81&&k<=0x84&&actions.label[k-0x81]){key_pop_all();return k-0x80;}
        if(k==27||k==0xb1||k==0x85){key_pop_all();return 0;}
        if(k==0xb5&&scroll)--scroll;
        else if(k==0xb6&&scroll+visible<=total)++scroll;
        else if(k==0xb4)scroll=scroll>=visible?scroll-visible:0;
        else if(k==0xb7&&total>=visible)scroll=std::min(scroll+visible,total-visible+1);
        if(previous!=scroll)break;
        }
    }
}

// One result screen: the value stays visible beside its available actions.
// Return true only for the optional secondary action; viewing preserves the stack.
bool value_result(object_p object,const char *title,const char *context="",bool secondary_action=false,const char *secondary="Plot")
{
    key_pop_all();object_g value=object;if(!value){engine_error();return false;}
    grapher graph(300,128,Settings.ResultFont(),grob::pattern::black,grob::pattern::white,false,true);
    graph.duration=250;grob_g image;
    do{image=value->graph(graph);}while(!image&&!rt.error()&&graph.reduce_font());
    if(rt.error())rt.clear_error();
    algebraic_g approximate=pico_result_approximation(value);if(rt.error())rt.clear_error();
    bool copy_approx=false;char approximation[192]="";if(approximate){size_t n=approximate->render(approximation,sizeof approximation);n=std::min(n,sizeof approximation-1);while(n&&!pico_text_editor::valid_utf8(approximation,n))--n;approximation[n]=0;}
    bool copied=false;size_t scroll=0;unsigned frame=~0u;
    for(;;)
    {
        bool chrome=content_page(title,pico_ui::Actions{{"Copy","Details",secondary_action?secondary:nullptr,approximate?(copy_approx?"Original":"Approx"):nullptr,"Back"}},frame);
        if(image)
        {
            auto pixels=image->pixels();int x=(320-pixels.width())/2,y=40+(128-pixels.height())/2;
            Screen.draw(pixels,x,y,pico_ui::text_colour());
            Screen.draw_background(pixels,x,y,pattern(Settings.Background()));
        }
        else
        {
            char text[192];size_t n=value->render(text,sizeof text);n=std::min(n,sizeof text-1);
            while(n&&!pico_text_editor::valid_utf8(text,n))--n;text[n]=0;
            if(rt.error())rt.clear_error();
            pico_information_text::draw(Screen,native_compact_font(),text,n,0,8,52,304,16,6,0,pico_ui::text_colour());
        }
        if(!image)info_label(8,161,"Full value: F2 Details");
        Screen.fill(8,178,311,178,pattern(70,85,100));
        if(approximate){char line[208];snprintf(line,sizeof line,"Approx: %s",approximation);ui_line(8,186,304,20,line,pico_ui::TextRole::status,pico_ui::text_colour());}
        size_t total=pico_information_text::draw(Screen,native_compact_font(),context,strlen(context),scroll,8,212,304,16,3,0,pico_ui::text_colour());
        if(copied)info_label(8,274,"Copied to stack");
        else if(copy_approx)info_label(8,274,"F1 copies approximation; F4 restores original");
        else if(total>=3)info_label(8,274,"Up/Down: more details");
        content_refresh(chrome,frame);int key=read_key_raw();
        if(key==27||key==0xb1||key==0x85){key_pop_all();return false;}
        if(key==0x81){if(rt.push(copy_approx?object_p(+approximate):object_p(+value)))copied=true;else engine_error();}
        else if(key==0x82)
        {
            object_g detail=copy_approx?object_p(+approximate):object_p(+value);text_g text=detail->as_text(true);
            if(!text){engine_error();continue;}
            size_t length;utf8 source=text->value(&length);char display[2048];size_t n=std::min(length,sizeof display-1);
            while(n&&!pico_text_editor::valid_utf8(reinterpret_cast<const char *>(source),n))--n;
            memcpy(display,source,n);display[n]=0;
            view_text(n<length?"Result details (shortened)":"Result details",display,n,pico_ui::TextRole::table);
        }
        else if(key==0x83&&secondary_action){key_pop_all();return true;}
        else if(key==0x84&&approximate){copy_approx=!copy_approx;copied=false;frame=~0u;}
        else if(key==0xb5&&scroll)--scroll;
        else if(key==0xb6&&scroll+3<=total)++scroll;
    }
}

