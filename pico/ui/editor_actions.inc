// Shared source-editor actions; bounded storage and no calculator evaluation.
bool editor_jump(pico_text_editor &editor,unsigned line)
{
    if(!line)return false;size_t at=0;unsigned row=1;
    while(row<line&&at<editor.length)if(editor.text[at++]=='\n')++row;
    if(row!=line)return false;editor.cursor=at;return true;
}
bool editor_error(const char *title,const char *message,unsigned line,const char *action="Edit at line")
{
    char text[384];if(line)snprintf(text,sizeof text,!strcmp(action,"Find")?"Command match: line %u\n\n%s":"Line %u\n\n%s",line,message);else snprintf(text,sizeof text,"%s\n\nNo source line is available. Your source is unchanged.",message);
    return screen_text("editor.error",title,text,strlen(text),pico_ui::TextRole::prose,{{line?action:nullptr,nullptr,nullptr,nullptr,"Back"}})==1;
}
void editor_insert_command(source_language language,pico_editor_tools &tools)
{
    char query[48]="";int selected=0;
    for(;;){
        const pico_editor_commands::Command *visible[8]={};int total=0,first=selected/8*8;
        for(const auto &command:pico_editor_commands::entries)if(command.language==unsigned(language)&&pico_reference::contains(command.name,query)){
            if(total>=first&&total<first+8)visible[total-first]=&command;++total;
        }
        screen_page("editor.commands","Insert command",{{total?"Choose":nullptr,first?"Previous":nullptr,first+8<total?"Next":nullptr,"Find","Back"}});
        for(int n=0;n<8&&first+n<total;++n){char label_text[96];snprintf(label_text,sizeof label_text,"%d  %s",n+1,visible[n]->name);label(8,37+n*27,label_text,selected==first+n);}
        if(!total)info_label(8,65,"No match. F4 changes the search.");list_page_hint(selected,total,true);flush_display();int key=read_key_raw();
        if(key==0x85||key==27||key==0xb1)return;
        if(key==0x84){char next[sizeof query];strcpy(next,query);if(prompt("Find command",next,sizeof next,false,true)){strcpy(query,next);selected=0;}continue;}
        if(!total)continue;
        if(key==0xb5){selected=(selected+total-1)%total;continue;}
        if(key==0xb6){selected=(selected+1)%total;continue;}
        if(key==0xb4||key==0x82){selected=std::max(0,first-8);continue;}
        if(key==0xb7||key==0x83){if(first+8<total)selected=first+8;continue;}
        int choice=-1;if(key==0x81||key==13||key==10)choice=selected-first;
        else if(key>='1'&&key<='8'&&first+key-'1'<total){choice=key-'1';selected=first+choice;}
        if(choice<0)continue;const auto &command=*visible[choice];
        const char *guidance=language==source_language::rpl?"Insert this command into your program.":language==source_language::lua?"Edit the call's arguments for your script.":"Edit the operands for your program.";
        char preview[512];snprintf(preview,sizeof preview,"%s\n\n%s Selected text will be replaced. Ctrl+Z undoes the insertion.",command.text,guidance);
        if(screen_text("editor.command_preview",command.name,preview,strlen(preview),pico_ui::TextRole::prose,{{"Insert",nullptr,nullptr,nullptr,"Back"}})!=1)continue;
        tools.break_group();size_t selected_bytes=tools.last()-tools.first();
        if(selected_bytes>pico_editor_tools::limit){notice("Selection too long","Select at most 256 bytes, or clear the selection and insert at the cursor.");continue;}
        if(strlen(command.text)>tools.e.capacity-1-(tools.e.length-selected_bytes)){notice("Source full","Remove some text before inserting. Your source is unchanged.");continue;}
        size_t start=tools.first();
        if(tools.insert(command.text,strlen(command.text))){tools.e.cursor=start+command.cursor+command.selection;tools.anchor=command.selection?start+command.cursor:pico_editor_tools::none;tools.history[tools.applied-1].new_cursor=tools.e.cursor;return;}
        notice("Undo storage unavailable","Save your source, then close and reopen the editor. Nothing was inserted.");
    }
}
