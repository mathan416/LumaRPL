// Native notebook editor; calculation is always an explicit action.
namespace {
pico_notebook *notebook=nullptr;
pico_document_cache<pico_notebook> notebook_cache;
void notebook_run(unsigned through)
{
    screen_page("notebook_ui.notebook_run.page1","Running notebook",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
    int failed;bool cancelled;
    auto poll=[](void *)->bool{return sampling_poll(nullptr,-1);};
    object_g result=pico_notebook_run(*notebook,through,failed,cancelled,poll);
    if(!result)
    {
        if(cancelled){rt.clear_error();notice("Notebook interrupted","Later cells were not run");}
        else if(failed>=0)
        {
            char title[48];snprintf(title,sizeof title,"Notebook stopped at cell %d",failed+1);
            screen_text("notebook_ui.notebook_run.view_text1",title,notebook->cells[failed].preview,strlen(notebook->cells[failed].preview));rt.clear_error();key_pop_all();
        }
        else if(rt.error())engine_error();
        else notice("Notebook","Unable to run this notebook");
        return;
    }
    char title[48];snprintf(title,sizeof title,"Notebook result R%u",through+1);document_result(result,title);
}
void notebook_files()
{
    if(!sys_disk_ok()){notice("Notebooks","SD card unavailable; contents remain in RAM");return;}
    if(pico_f_mkdir("notebooks")!=FR_OK&&!file_exists("notebooks")){notice("Notebooks","Cannot create directory");return;}
    static const char *const items[]={"Save notebook","Load notebook","Back"};int action=screen_menu("notebook_ui.notebook_files.choose1","Notebook files",items,3);if(action<0||action==2)return;
    char path[320];
    if(action==0)
    {
        if(!document_save_name("notebooks",".notebook",path,sizeof path))return;
        if(pico_notebook_save(path,*notebook)){notebook->dirty=false;notice("Notebook saved","Expressions and notes saved; results are temporary");}
        else notice("Notebook save failed","Previous file retained; contents remain in RAM");
        return;
    }
    if(notebook->dirty&&!confirm("Replace unsaved notebook changes?"))return;
    if(!document_file("notebooks",".notebook",path,sizeof path))return;
    auto *candidate=new(std::nothrow) pico_notebook;
    if(!candidate){notice("Notebook","Not enough memory to load safely");return;}
    bool backup=false;
    if(pico_notebook_load(path,*candidate,backup))
    { *notebook=*candidate;notice(backup?"Notebook backup loaded":"Notebook loaded","Results are cleared; choose Run explicitly"); }
    else notice("Notebook load failed","Primary and backup invalid/unavailable; current kept");
    delete candidate;
}
void notebook_cell(unsigned index)
{
    for(;;)
    {
        char title[48];snprintf(title,sizeof title,"Cell %u -> R%u",index+1,index+1);
        static const char *const items[]={"Edit expression","Edit note","View note / last result","Run through this cell","Move cell up","Delete cell","Back"};
        int action=screen_menu("notebook_ui.notebook_cell.choose1",title,items,7);if(action<0||action==6)return;
        if(action==0)
        {
            char expression[160];snprintf(expression,sizeof expression,"%s",notebook->cells[index].expression);
            if(formula_entry("Notebook expression",expression,false,false,false,"Expression; R1..R12 earlier results")&&!notebook->set_expression(index,expression))notice("Notebook","Invalid expression or cell index");
        }
        if(action==1)
        {
            char note[96];snprintf(note,sizeof note,"%s",notebook->cells[index].note);
            if(prompt("Cell note (95 bytes)",note,sizeof note,false,true)&&!notebook->set_note(index,note))notice("Notebook","Invalid note");
        }
        if(action==2)
        {
            const auto &cell=notebook->cells[index];char text[768];
            snprintf(text,sizeof text,"Expression:\n%s\n\nNote:\n%s\n\nLast result:\n%s",cell.expression,cell.note,*cell.preview?cell.preview:"Not run / invalidated");
            screen_text("notebook_ui.notebook_cell.view_text1",title,text,strlen(text));
        }
        if(action==3)notebook_run(index);
        if(action==4)
        {
            if(!index){notice("Notebook","This is already the first cell");continue;}
            auto previous=notebook->cells[index-1];notebook->cells[index-1]=notebook->cells[index];notebook->cells[index]=previous;notebook->invalidate(index-1);return;
        }
        if(action==5&&confirm("Delete this cell?"))
        {
            if(!notebook->erase(index)){notice("Notebook","Keep at least one cell");continue;}return;
        }
    }
}
void notebook_cells()
{
    for(;;)
    {
        char labels[pico_notebook::capacity+1][72];const char *items[pico_notebook::capacity+1];
        for(unsigned i=0;i<notebook->count;++i)
        {
            const auto &cell=notebook->cells[i];char expression[42];pico_formula_label(expression,sizeof expression,cell.expression);
            const char *state=cell.status==pico_notebook::state::complete?"ok":cell.status==pico_notebook::state::error?"error":cell.status==pico_notebook::state::cancelled?"stop":"--";
            snprintf(labels[i],sizeof labels[i],"%u [%s] %s",i+1,state,expression);items[i]=labels[i];
        }
        items[notebook->count]="Back";int selected=screen_menu("notebook_ui.notebook_cells.choose1","Notebook cells",items,notebook->count+1);
        if(selected<0||unsigned(selected)>=notebook->count)return;notebook_cell(unsigned(selected));
    }
}
}
void pico_notebooks_menu()
{
    if(rt.editing()){notice("Notebooks","Finish or cancel calculator entry first");return;}
    pico_document_access<pico_notebook> access(notebook_cache,notebook);
    if(!notebook){notice("Notebooks","Document unavailable or corrupt PSRAM");return;}
    save<bool> direct_screen(user_display_enable,false);
    for(;;)
    {
        char title[80];snprintf(title,sizeof title,"%s%s",notebook->dirty?"* ":"",notebook->title);
        static const char *const items[]={"Cells / expressions / notes","Run all cells","Add cell","Files: save / load","Edit notebook title","Reset to demo","Help","Back"};
        int action=screen_menu("notebook_ui.pico_notebooks_menu.choose1",title,items,8);if(action<0||action==7)break;
        if(action==0)notebook_cells();
        if(action==1)notebook_run(notebook->count-1);
        if(action==2){if(!notebook->append())notice("Notebook full","Maximum 12 cells");else notebook_cell(notebook->count-1);}
        if(action==3)notebook_files();
        if(action==4)
        {
            char next[48];snprintf(next,sizeof next,"%s",notebook->title);
            if(prompt("Notebook title",next,sizeof next)&&pico_notebook::text(next,sizeof next)){snprintf(notebook->title,sizeof notebook->title,"%s",next);notebook->dirty=true;}
        }
        if(action==5&&(!notebook->dirty||confirm("Replace unsaved notebook with demo?"))){*notebook=pico_notebook();notebook->dirty=true;}
        if(action==6)
        {
            const char *text=pico_ui_help::tool_text("notebook_ui.pico_notebooks_menu.view_text1");screen_text("notebook_ui.pico_notebooks_menu.view_text1","Notebook help",text,strlen(text));
        }
    }
    key_pop_all();
}
