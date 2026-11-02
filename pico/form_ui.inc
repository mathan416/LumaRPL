// Reusable named-input forms; included after document_browser.inc.
namespace {
pico_form_document *active_form=nullptr;
pico_document_cache<pico_form_document> active_form_cache;
bool form_validate_syntax(const pico_form_document &form)
{
    if(!form.valid()){rt.value_error();return false;}
    cleaner temporary;expression_g equation=pico_formula_parse(form.formula);if(!equation)return false;
    for(unsigned i=0;i<form.count;++i){if(!pico_form_name(form.fields[i].name))return false;expression_g input=pico_formula_parse(form.fields[i].input);if(!input)return false;}
    return true;
}
bool form_replace_current()
{return !active_form->dirty||confirm("Discard unsaved form changes?");}
void form_field_edit(unsigned selected)
{
    for(;;)
    {
        auto &field=active_form->fields[selected];
        static const char *const items[]={"Edit input expression","Rename input variable","Delete input","Input units (optional)","Back"};
        int action=screen_menu("form_ui.form_field_edit.choose1",field.name,items,5);if(action<0||action==4)return;
        if(action==0)
        {
            char next[64];snprintf(next,sizeof next,"%s",field.input);
            if(!prompt("Input expression (max 63 bytes)",next,sizeof next))continue;
            cleaner temporary;expression_g equation=pico_formula_parse(next);if(!equation){engine_error();continue;}
            snprintf(field.input,sizeof field.input,"%s",next);active_form->dirty=true;
        }
        if(action==1)
        {
            pico_form_document next=*active_form;
            if(!prompt("Variable name (max 12 chars)",next.fields[selected].name,sizeof field.name))continue;
            if(!pico_form_name(next.fields[selected].name)){engine_error();continue;}
            if(!next.valid()){notice("Input name","Use unique names, starting with a letter");continue;}
            next.dirty=true;*active_form=next;notice("Input renamed","Update formula references if needed");
        }
        if(action==3){char units[32];snprintf(units,sizeof units,"%s",field.units);if(prompt("Units, e.g. V, ohm, m/s (empty=none)",units,sizeof units)){if(*units){cleaner temporary;unit_g check=pico_form_unit(units);if(!check){engine_error();continue;}}strcpy(field.units,units);active_form->dirty=true;}}
        if(action==2&&confirm("Delete this input?"))
        {
            memmove(active_form->fields+selected,active_form->fields+selected+1,(active_form->count-selected-1)*sizeof field);
            --active_form->count;active_form->fields[active_form->count]=pico_form_document::field{};active_form->dirty=true;return;
        }
    }
}
void form_fields()
{
    for(;;)
    {
        const char *items[8];char labels[6][80];unsigned count=active_form->count;
        for(unsigned i=0;i<count;++i){char snippet[46];pico_formula_label(snippet,sizeof snippet,active_form->fields[i].input);snprintf(labels[i],sizeof labels[i],"%s = %s%s%s",active_form->fields[i].name,snippet,*active_form->fields[i].units?" _":"",active_form->fields[i].units);items[i]=labels[i];}
        items[count]="Add input";items[count+1]="Back";
        int action=screen_menu("form_ui.form_fields.choose1","Form inputs (maximum six)",items,count+2);if(action<0||unsigned(action)==count+1)return;
        if(unsigned(action)<count){form_field_edit(action);continue;}
        if(count==6){notice("Form inputs","Maximum six inputs reached");continue;}
        pico_form_document next=*active_form;auto &field=next.fields[count];snprintf(field.name,sizeof field.name,"INPUT%u",count+1);strcpy(field.input,"0");
        if(!prompt("New variable name",field.name,sizeof field.name)||!prompt("Input expression",field.input,sizeof field.input))continue;
        ++next.count;if(!form_validate_syntax(next)){engine_error();continue;}
        next.dirty=true;*active_form=next;
    }
}
int form_plot_sample(void *opaque,int,double x,double,double *y)
{return static_cast<pico_form_curve *>(opaque)->sample(x,*y);}
void form_graph()
{
    if(!active_form->count){notice("Form graph","Add an input to sweep first");return;}
    const char *names[7];for(unsigned i=0;i<active_form->count;++i)names[i]=active_form->fields[i].name;names[active_form->count]="Back";
    int selected=screen_menu("form_ui.form_graph.choose1","Choose input for horizontal axis",names,active_form->count+1);if(selected<0||unsigned(selected)>=active_form->count)return;
    double window[4]={0,10,0,20};static const char *const titles[]={"Input minimum (X)","Input maximum (X)","Result minimum (Y)","Result maximum (Y)"};
    for(int i=0;i<4;++i)if(!analysis_number(titles[i],window[i]))return;
    if(!pico_graph_scene::bounds(window)){notice("Form graph","Use ordered finite bounds");return;}
    pico_form_curve curve{*active_form,selected};if(!curve.prepare()){engine_error();return;}pico_plot_request request{};request.active[0]=1;request.colour[0]=0x3080ef;memcpy(request.window,window,sizeof window);
    char xunits[48]{},yunits[48]{};if(curve.xunit)curve.xunit->uexpr()->render(xunits,sizeof xunits-1);if(curve.yunit)curve.yunit->uexpr()->render(yunits,sizeof yunits-1);
    snprintf(request.label[0],80,"Result%s%.20s%s vs %.12s%s%.20s%s",*yunits?"(":"",yunits,*yunits?")":"",active_form->fields[selected].name,*xunits?"(":"",xunits,*xunits?")":"");
    if(*xunits)notice("Horizontal axis unit",xunits);if(*yunits)notice("Result axis unit",yunits);
    int result=plot_curves(request,{form_plot_sample,&curve});if(result==-1)engine_error();if(result==-2)rt.clear_error();
}
void form_files()
{
    if(!sys_disk_ok()){notice("Forms","SD card unavailable");return;}pico_f_mkdir("forms");
    static const char *const items[]={"Save form","Load form","Back"};int action=screen_menu("form_ui.form_files.choose1","Form files",items,3);if(action<0||action==2)return;
    char path[320];
    if(action==0)
    {
        if(!document_save_name("forms",".form",path,sizeof path))return;
        bool ok=pico_form_save(path,*active_form);if(ok)active_form->dirty=false;
        notice(ok?"Form saved":"Form save failed",ok?"Stored under /db48x/forms":"Previous file retained");
    }
    else
    {
        if(!document_file("forms",".form",path,sizeof path))return;
        pico_form_document candidate;bool backup=false;
        if(!pico_form_load(path,candidate,backup)){notice("Invalid form","Primary and backup unavailable; current kept");return;}
        if(!form_validate_syntax(candidate)){engine_error();return;}
        if(!form_replace_current())return;*active_form=candidate;
        notice(backup?"Backup form loaded":"Form loaded","Choose Calculate to evaluate");
    }
}
}
void pico_forms_menu()
{
    if(rt.editing()){notice("Forms","Finish or cancel the calculator entry");return;}
    pico_document_access<pico_form_document> access(active_form_cache,active_form);
    if(!active_form){notice("Forms","Document unavailable (memory / PSRAM)");return;}
    save<bool> direct_screen(user_display_enable,false);
    for(;;)
    {
        char title[80];snprintf(title,sizeof title,"Form: %.47s%s",active_form->title,active_form->dirty?" *":"");
        static const char *const items[]={"Edit title","Edit result formula","Inputs / values","Calculate","Graph an input sweep","Save / load forms","Examples / help","Result units (optional)"};
        int action=screen_menu("form_ui.pico_forms_menu.choose1",title,items,8);if(action<0)break;
        if(action==7)action=8;
        else if(action==6){static const char *const help[]={"Examples","Help"};int choice=screen_menu("form_ui.examples_help","Form examples / help",help,2);if(choice<0)continue;action=choice==0?6:7;}
        if(action==0){char next[48];snprintf(next,sizeof next,"%s",active_form->title);if(prompt("Form title",next,sizeof next)){snprintf(active_form->title,sizeof active_form->title,"%s",next);active_form->dirty=true;}}
        if(action==1){char next[160];snprintf(next,sizeof next,"%s",active_form->formula);if(formula_entry("Formula using named inputs",next,false,false,false,"Formula using input names")){snprintf(active_form->formula,sizeof active_form->formula,"%s",next);active_form->dirty=true;}}
        if(action==2)form_fields();
        if(action==3)
        {
            screen_page("form_ui.pico_forms_menu.page1","Calculating form",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();object_g result=pico_form_calculate(*active_form);
            if(!result){if(rt.error())engine_error();else notice("Form","Calculation interrupted or unavailable");}
            else document_result(result,active_form->title);
        }
        if(action==4)form_graph();
        if(action==5)form_files();
        if(action==6)
        {
            static const char *const examples[]={"Ohm's law: voltage/resistance","Circle area","Ohm with V / ohm -> A","Back"};int choice=screen_menu("form_ui.pico_forms_menu.choose2","Form examples",examples,4);
            if(choice>=0&&choice<3&&form_replace_current()){*active_form=pico_form_document();if(choice==1)active_form->circle();if(choice==2){strcpy(active_form->fields[0].units,"V");strcpy(active_form->fields[1].units,"ohm");strcpy(active_form->output_units,"A");}active_form->dirty=true;}
        }
        if(action==8){char units[32];snprintf(units,sizeof units,"%s",active_form->output_units);if(prompt("Result units (empty=keep native units)",units,sizeof units)){if(*units){cleaner temporary;unit_g check=pico_form_unit(units);if(!check){engine_error();continue;}}strcpy(active_form->output_units,units);active_form->dirty=true;}}
        if(action==7)
        {
            const char *text=pico_ui_help::tool_text("form_ui.pico_forms_menu.view_text1");screen_text("form_ui.pico_forms_menu.view_text1","Formula form help",text,strlen(text));
        }
    }
    key_pop_all();
}
