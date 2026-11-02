// First-order initial-value problems, using the shared native renderer.
namespace {
pico_ui::Revision ode_revision;
pico_ode_scene *ode_scene=nullptr;
pico_document_cache<pico_ode_scene> ode_scene_cache;
int ode_sample(void *context,int,double x,double,double *y){return static_cast<pico_ode_context *>(context)->sample(x,*y);}
void ode_files()
{
    if(!sys_disk_ok()){notice("ODE files","SD card unavailable");return;}pico_f_mkdir("odes");
    static const char *const items[]={"Save equation","Load equation","Back"};int action=screen_menu("ode_ui.ode_files.choose1","Differential equation files",items,3);if(action<0||action==2)return;char path[320];
    if(action==0){if(!document_save_name("odes",".ode",path,sizeof path))return;ode_scene->angle=angle_number();bool ok=pico_ode_save(path,*ode_scene);if(ok)ode_revision.saved();notice(ok?"ODE saved":"Save failed",ok?"Stored under /db48x/odes":"Previous file retained");}
    else if(document_file("odes",".ode",path,sizeof path))
    {
        pico_ode_scene candidate;bool backup=false;if(!pico_ode_load(path,candidate,backup)){notice("Invalid ODE","Primary and backup unavailable; current kept");return;}
        {cleaner temporary;expression_g equation=pico_formula_parse(candidate.formula);if(!equation){engine_error();return;}}
        if(ode_revision.dirty&&!confirm("Replace unsaved equation changes?"))return;if(!confirm("Load equation and its angle mode?"))return;*ode_scene=candidate;ode_revision.saved();set_scene_angle(candidate.angle);notice(backup?"ODE backup loaded":"ODE loaded","Choose Plot solution / field");
    }
}
}
void pico_ode_menu()
{
    bool first=!ode_scene_cache.resident&&!ode_scene_cache.handle;
    pico_document_access<pico_ode_scene> access(ode_scene_cache,ode_scene);
    if(first&&ode_scene){ode_scene->angle=angle_number();}
    if(!ode_scene){notice("ODE","Document unavailable (memory / PSRAM)");return;}save<bool> direct_screen(user_display_enable,false);
    for(;;)
    {
        pico_ui::Mutation<pico_ode_scene> mutation(*ode_scene,ode_revision);
        static const char *const items[]={"Plot solution / direction field","Edit dy/dX = f(X,Y)","Initial condition X0, Y0","View window","Examples","Save / load equations","Help","Systems / second order"};int action=screen_menu("ode_ui.pico_ode_menu.choose1","Differential equations",items,8);if(action<0)break;
        if(action==7)advanced_launch(1);
        if(action==0)
        {
            ode_scene->angle=angle_number();pico_ode_context context(*ode_scene);if(!context.valid()){engine_error();continue;}
            pico_plot_request request{};request.active[0]=1;request.colour[0]=0x3080ef;memcpy(request.window,ode_scene->window,sizeof request.window);snprintf(request.label[0],80,"dy/dX=%.70s",ode_scene->formula);
            int code=plot_curves(request,{ode_sample,&context,nullptr,nullptr,nullptr,nullptr,&context});memcpy(ode_scene->window,request.window,sizeof request.window);if(code==-1)engine_error();if(code==-2)rt.clear_error();
        }
        if(action==1)formula_entry("dy/dX = f(X,Y)",ode_scene->formula,false,false,false,"Enter slope in X,Y (no dy/dX=)","X,Y");
        if(action==2){double x=ode_scene->x0,y=ode_scene->y0;if(analysis_number("Initial X0",x)&&analysis_number("Initial Y0",y)){ode_scene->x0=x;ode_scene->y0=y;}}
        if(action==3){double next[4];memcpy(next,ode_scene->window,sizeof next);static const char *const titles[]={"X minimum","X maximum","Y minimum","Y maximum"};bool ok=true;for(int i=0;i<4&&ok;++i)ok=analysis_number(titles[i],next[i]);if(ok){if(pico_graph_scene::bounds(next))memcpy(ode_scene->window,next,sizeof next);else notice("Window","Use ordered finite bounds");}}
        if(action==4)
        {
            static const char *const examples[]={"Exponential: y'=y","Logistic: y'=y*(1-y)","Driven: y'=X-Y","Back"};int which=screen_menu("ode_ui.pico_ode_menu.choose2","ODE examples",examples,4);if(which<0||which==3)continue;*ode_scene=pico_ode_scene();ode_scene->angle=angle_number();
            if(which==1){strcpy(ode_scene->formula,"Y*(1-Y)");ode_scene->y0=.1;double w[]={-2,8,-.2,1.2};memcpy(ode_scene->window,w,sizeof w);}
            if(which==2){strcpy(ode_scene->formula,"X-Y");double w[]={-2,4,-2,5};memcpy(ode_scene->window,w,sizeof w);}
        }
        if(action==5)ode_files();
        if(action==6){const char *help=pico_ui_help::tool_text("ode_ui.pico_ode_menu.view_text1");screen_text("ode_ui.pico_ode_menu.view_text1","Differential equation help",help,strlen(help));}
    }
    key_pop_all();
}
