namespace {
pico_ui::Revision path_revision;
pico_path_scene *path_scene=nullptr;
pico_document_cache<pico_path_scene> path_scene_cache;
int path_sample(void *opaque,double t,double *x,double *y)
{return static_cast<pico_path_sampler *>(opaque)->sample(t,*x,*y);}
int path_component(void *opaque,int component,double t,double,double *value)
{
    double x,y;int result=path_sample(opaque,t,&x,&y);if(result==1)*value=component?y:x;return result;
}
void path_plot()
{
    path_scene->angle=angle_number();pico_path_sampler sampler(*path_scene);
    if(!sampler.valid()){engine_error();return;}
    pico_plot_request request{};request.active[0]=1;request.colour[0]=0x3080ef;memcpy(request.window,path_scene->window,sizeof request.window);
    plot_label(request.label[0],80,path_scene->polar?path_scene->radius:path_scene->x);
    int result=plot_curves(request,{path_component,&sampler,nullptr,nullptr,path_sample,path_scene});
    memcpy(path_scene->window,request.window,sizeof request.window);if(result==-1)engine_error();if(result==-2)rt.clear_error();
}
void path_files()
{
    if(!sys_disk_ok()){notice("Paths","SD card unavailable");return;}pico_f_mkdir("paths");
    static const char *const items[]={"Save path","Load path","Back"};int action=screen_menu("path_ui.path_files.choose1","Parametric / polar files",items,3);if(action<0||action==2)return;char path[320];
    if(action==0)
    {
        if(!document_save_name("paths",".path",path,sizeof path))return;path_scene->angle=angle_number();
        bool ok=pico_path_save(path,*path_scene);if(ok)path_revision.saved();notice(ok?"Path saved":"Path save failed",ok?"Stored under /db48x/paths":"Previous file retained");
    }
    else if(document_file("paths",".path",path,sizeof path))
    {
        pico_path_scene candidate;bool backup=false;
        if(!pico_path_load(path,candidate,backup)){notice("Invalid path","Primary and backup unavailable; current kept");return;}
        {cleaner temporary;expression_g a=pico_formula_parse(candidate.x),b=pico_formula_parse(candidate.y),r=pico_formula_parse(candidate.radius);if(!a||!b||!r){engine_error();return;}}
        if(path_revision.dirty&&!confirm("Replace unsaved path changes?"))return;if(!confirm("Load path and its angle mode?"))return;*path_scene=candidate;path_revision.saved();set_scene_angle(candidate.angle);
        notice(backup?"Path backup loaded":"Path loaded","Choose Plot path");
    }
}
}
void pico_path_menu()
{
    bool first=!path_scene_cache.resident&&!path_scene_cache.handle;
    pico_document_access<pico_path_scene> access(path_scene_cache,path_scene);
    if(first&&path_scene){path_scene->angle=angle_number();path_scene->last=pico_path_scene::turn(path_scene->angle);}
    if(!path_scene){notice("Paths","Document unavailable (memory / PSRAM)");return;}
    save<bool> direct_screen(user_display_enable,false);
    for(;;)
    {
        pico_ui::Mutation<pico_path_scene> mutation(*path_scene,path_revision);
        const char *items[]={"Plot path",path_scene->polar?"Mode: polar r(T)":"Mode: parametric X(T),Y(T)",path_scene->polar?"Edit r(T)":"Edit X(T)","Edit Y(T) (parametric)","T interval","View window","Examples / help","Save / load paths"};
        int action=screen_menu("path_ui.pico_path_menu.choose1","Parametric / polar",items,8);if(action<0)break;
        if(action==6){static const char *const help[]={"Examples","Help"};int choice=screen_menu("path_ui.examples_help","Path examples / help",help,2);if(choice<0)continue;action=choice==0?6:8;}
        if(action==0)path_plot();
        if(action==1){static const char *const modes[]={"Parametric X(T),Y(T)","Polar r(T)","Back"};int mode=screen_menu("path_ui.pico_path_menu.choose2","Path mode",modes,3);if(mode>=0&&mode<2)path_scene->polar=mode;}
        if(action==2){char *target=path_scene->polar?path_scene->radius:path_scene->x;formula_entry("Formula using T",target,false,false,false,"Formula in T (no X= or r=)","T");}
        if(action==3){if(path_scene->polar)notice("Polar mode","Only r(T) is used; switch mode to edit Y(T)");else formula_entry("Y formula using T",path_scene->y,false,false,false,"Formula in T (no Y=)","T");}
        if(action==4)
        {
            double first=path_scene->first,last=path_scene->last;
            if(analysis_number("First T",first)&&analysis_number("Last T",last)){if(first>=last)notice("T interval","Require first < last");else{path_scene->first=first;path_scene->last=last;}}
        }
        if(action==5)
        {
            static const char *const titles[]={"X minimum","X maximum","Y minimum","Y maximum"};double next[4];memcpy(next,path_scene->window,sizeof next);bool ok=true;
            for(int i=0;i<4&&ok;++i)ok=analysis_number(titles[i],next[i]);
            if(ok){if(pico_graph_scene::bounds(next))memcpy(path_scene->window,next,sizeof next);else notice("Window","Use ordered finite bounds");}
        }
        if(action==6)
        {
            static const char *const examples[]={"Circle (parametric)","Ellipse (parametric)","Rose (polar)","Spiral (polar)","Back"};int selected=screen_menu("path_ui.pico_path_menu.choose3","Path examples",examples,5);
            if(selected<0||selected==4||!confirm("Use radians and replace current path?"))continue;
            *path_scene=pico_path_scene();set_scene_angle(1);
            if(selected==1)strcpy(path_scene->x,"2*COS(T)");
            if(selected>=2)path_scene->polar=1;
            if(selected==3){strcpy(path_scene->radius,"T/(2*PI)");path_scene->last=12.566370614359172;}
        }
        if(action==7)path_files();
        if(action==8)
        {
            const char *text=pico_ui_help::tool_text("path_ui.pico_path_menu.view_text1");screen_text("path_ui.pico_path_menu.view_text1","Parametric / polar help",text,strlen(text));
        }
    }
    key_pop_all();
}
