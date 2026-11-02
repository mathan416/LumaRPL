namespace {
struct reference_book { const char *title,*index,*text; const pico_reference_data::Category *categories; unsigned count; bool guided=false; };
constexpr reference_book rpl_book={"Command reference","help/commands.lri","help/commands.lrt",pico_reference_data::categories,sizeof pico_reference_data::categories/sizeof *pico_reference_data::categories};
constexpr reference_book lua_book={"Lua reference","help/lua.lri","help/lua.lrt",pico_reference_data::lua_categories,sizeof pico_reference_data::lua_categories/sizeof *pico_reference_data::lua_categories,true};
constexpr reference_book chip8_book={"CHIP-8 reference","help/chip8.lri","help/chip8.lrt",pico_reference_data::chip8_categories,sizeof pico_reference_data::chip8_categories/sizeof *pico_reference_data::chip8_categories,true};
void command_reference_topic(pico_reference::store &store,const pico_reference::entry &entry)
{
    // Keep this off the nested menu stack and release it on every exit.
    char *text=static_cast<char *>(malloc(pico_reference::topic_bytes+1));
    if(!text){notice("Command reference","Not enough memory to open this page. Your calculation is unchanged.");return;}
    if(store.topic(entry,text,pico_reference::topic_bytes+1))screen_text("calculator.help",entry.title,text,entry.length);
    else notice("Cannot read help","The help files are missing or damaged. Copy the matching LumaRPL help folder to SD and try again.");
    free(text);
}
void command_reference_list(pico_reference::store &store,int category,const char *initial_query=nullptr,const reference_book &book=rpl_book)
{
    key_pop_all();char query[80]="";if(initial_query)snprintf(query,sizeof query,"%s",initial_query);
    pico_ui::MenuState state{};pico_reference::entry records[8];unsigned cached=~0u,total=0,shown=0;
    for(;;)
    {
        unsigned first=pico_native_menu::offset(state.selected);
        if(cached!=first)
        {
            shown=total=0;pico_reference::entry row;
            unsigned begin=0,end=store.count,known_total=0,exact=~0u;
            if(query[0]&&store.exact(query,row,&exact)){if(!first)records[shown++]=row;total=1;}
            if(!query[0]){unsigned base=0;known_total=store.count;if(category>=0){const auto &c=book.categories[category];base=c.first;known_total=c.count;}begin=base+first;end=std::min(base+known_total,begin+8);total=first;}
            for(unsigned i=begin;i<end;++i)
            {
                if(!store.read(i,row)){notice("Cannot read help","Please copy the matching help files to SD, then reopen Help.");return;}
                if(i==exact)continue;
                if(category>=0&&row.category!=unsigned(category))continue;
                if(query[0]&&!pico_reference::contains(row.keys,query))continue;
                if(total>=first&&shown<8)records[shown++]=row;
                ++total;
                // Category browsing knows its range; avoid scanning the rest.
                if(!query[0]&&shown==8)break;
            }
            if(!query[0])total=known_total;
            cached=first;
        }
        char title[96];snprintf(title,sizeof title,query[0]?"Commands: %s":"%s",query[0]?query:category>=0?book.categories[category].name:"All entries");
        screen_page("reference.commands",title,{{shown?"Open":nullptr,state.selected>=8?"Previous":nullptr,first+8<total?"Next":nullptr,"Find","Back"}});
        for(unsigned n=0;n<shown;++n){char number[]={char('1'+n),0};label(28,37+n*27,records[n].title,state.selected==int(first+n));ui_line(7,40+n*27,16,16,number,pico_ui::TextRole::status,pico_ui::text_colour());}
        if(!shown)info_label(8,65,"No matching commands. F4 changes the search.");
        list_page_hint(state.selected,total,true);flush_display();int key=read_key_raw();
        if(key==27||key==0xb1||key==0x85)return;
        if(key==0x84||key==11){char next[80];strcpy(next,query);if(prompt("Find command",next,sizeof next,false,true)){strcpy(query,next);category=-1;state.selected=0;cached=~0u;}continue;}
        if(!total)continue;
        int previous=state.selected;
        if(key==0xb5)state.selected=(state.selected+total-1)%total;
        else if(key==0xb6)state.selected=(state.selected+1)%total;
        else if((key==0xb4||key==0x82)&&state.selected>=8)state.selected=std::max(0,(state.selected/8-1)*8);
        else if((key==0xb7||key==0x83)&&first+8<total)state.selected=std::min(int(total)-1,(state.selected/8+1)*8);
        else
        {
            int selected=-1;if(key==13||key==10||key==0x81)selected=state.selected-first;
            else if(key>='1'&&key<='8'&&unsigned(key-'1')<shown){selected=key-'1';state.selected=first+selected;}
            if(selected>=0&&unsigned(selected)<shown){command_reference_topic(store,records[selected]);key_pop_all();}
        }
        if(previous!=state.selected&&pico_native_menu::offset(previous)!=pico_native_menu::offset(state.selected))cached=~0u;
    }
}
void native_command_reference(const char *topic=nullptr,bool find=false,const reference_book &book=rpl_book)
{
    save<bool> display(user_display_enable,false);key_pop_all();pico_reference::store store;
    if(!sys_disk_ok()||!store.open(book.index,book.text)){notice(book.title,"Insert the SD card with this release's help files, then try again. The built-in calculator guide still works without it.");return;}
    if(topic&&*topic&&strcasecmp(topic,"Overview")){pico_reference::entry e;if(store.exact(topic,e))command_reference_topic(store,e);else command_reference_list(store,-1,topic,book);key_pop_all();return;}
    if(find){char query[80]="";if(prompt("Find command",query,sizeof query,false,true))command_reference_list(store,-1,query,book);return;}
    const unsigned count=book.count;
    const unsigned first=book.guided?0:2,find_index=book.guided?count:0,all_index=book.guided?count+1:1;
    const char *items[34];items[find_index]="Find a command or topic";items[all_index]="All entries";for(unsigned i=0;i<count;++i)items[i+first]=book.categories[i].name;
    for(;;){int a=screen_menu("reference.subjects",book.title,items,count+2,nullptr,book.title);if(a<0)return;if(unsigned(a)==find_index){char query[80]="";if(prompt("Find command",query,sizeof query,false,true))command_reference_list(store,-1,query,book);}else command_reference_list(store,unsigned(a)==all_index?-1:a-first,nullptr,book);}
}
}