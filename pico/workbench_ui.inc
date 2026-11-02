// Included by desktop.cc to share the native menu/editor/plot implementation.
namespace {
pico_ui::Revision workbench_revision;
pico_workbench workbench;
int workbench_sample(void *opaque,int,double x,double,double *z)
{
    auto **context=static_cast<void **>(opaque);
    int result=static_cast<pico_workbench_sampler *>(context[0])->sample(x,static_cast<pico_workbench *>(context[1]),*z);
    if(result==-2)rt.clear_error();return result;
}
void workbench_failure()
{
    if(rt.error())engine_error();else notice("No verified solution","Check guesses / domain / equation residuals");
}
void workbench_use(const double *values,int count,int action=0)
{
    if(!action){static const char *const options[]={"Back","Copy value to stack","Store variable"};action=screen_menu("workbench_ui.workbench_use.choose1","Use verified result",options,3);}if(action<=0)return;
    int selected=0;
    if(count>1){static const char *const names[]={"X","Y","Z"};selected=screen_menu("workbench_ui.workbench_use.choose2","Choose variable",names,count);if(selected<0||selected>=count)return;}
    object_g value=hwdouble::make(values[selected]);if(!value){engine_error();return;}
    if(action==1){if(!rt.push(value))engine_error();}
    else{char name[2]={char('X'+selected),0};symbol_g symbol=symbol::make(name);if(!symbol||!directory::store_here(symbol,value))engine_error();else notice("Variable stored","Ctrl+S saves it with the calculator session");}
}
void workbench_numerical(bool system)
{
    double values[3],residuals[3];screen_page("workbench_ui.workbench_numerical.page1","Solving equations",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
    if(!pico_workbench_solve(workbench,system,values,residuals)){workbench_failure();return;}
    char text[800]="";size_t used=strlen(text);int count=system?workbench.count:1;
    for(int i=0;i<count;++i)used+=snprintf(text+used,sizeof text-used,"\n%c = %.12g\nScaled residual = %.3g\n",'X'+i,values[i],residuals[i]);
    used+=snprintf(text+used,sizeof text-used,"\nLocal solution from initial guesses.");
    int action=screen_text("workbench_ui.workbench_numerical.view_text1","Solver result",text,used,pico_ui::TextRole::prose,pico_ui::Actions{{"Copy","Store",nullptr,nullptr,"Back"}});if(action)workbench_use(values,count,action);
}
void workbench_plot(bool parameters)
{
    pico_workbench_sampler sampler(parameters?workbench.scene.curve[0]:workbench.single);
    if(!sampler.equation||!sampler.scope.entered){workbench_failure();return;}
    pico_plot_request request{};request.active[0]=1;memcpy(request.window,workbench.scene.window,sizeof request.window);
    request.colour[0]=workbench.scene.colour[0];plot_label(request.label[0],80,parameters?workbench.scene.curve[0]:"Equation residual (left-right)");
    void *context[]={&sampler,parameters?&workbench:nullptr};
    int result=plot_curves(request,{workbench_sample,context,parameters?&workbench:nullptr});
    memcpy(workbench.scene.window,request.window,sizeof request.window);
    if(result==-1)workbench_failure();
}
void workbench_interval()
{
    double a=workbench.interval[0],b=workbench.interval[1];
    if(!analysis_number("Search first X",a)||!analysis_number("Search last X",b))return;
    if(a>=b){notice("Search interval","First X must be less than last X");return;}workbench.interval[0]=a;workbench.interval[1]=b;
    pico_plot_analysis::result result;
    {
        pico_workbench_sampler sampler(workbench.single);if(!sampler.equation||!sampler.scope.entered){workbench_failure();return;}
        void *context[]={&sampler,nullptr};screen_page("workbench_ui.workbench_interval.page1","Searching interval",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
        result=pico_plot_analysis::run(pico_plot_analysis::operation::roots,workbench_sample,context,0,0,a,b,0,sampling_poll,nullptr,8160);
        if(result.state==pico_plot_analysis::status::ok)
            for(int i=0;i<result.count;++i){result.evaluations+=2;double residual;if(!sampler.scope.bind("X",result.points[i].x)||!pico_workbench_check(workbench.single,residual)){result.state=pico_plot_analysis::status::not_converged;break;}}
    }
    if(result.state!=pico_plot_analysis::status::ok){if(rt.error())engine_error();else notice("Interval search",pico_plot_analysis::message(result.state));return;}
    char text[1400];size_t used=snprintf(text,sizeof text,"%d roots resolved in %.8g .. %.8g\nNot a guarantee of every root.\n",result.count,a,b);
    for(int i=0;i<result.count;++i)used+=snprintf(text+used,sizeof text-used,"\n%d: X=%.12g",i+1,result.points[i].x);
    used+=snprintf(text+used,sizeof text-used,"%s",result.truncated?"\nFirst 16 roots shown.":"");int action=screen_text("workbench_ui.workbench_interval.view_text1","Interval results",text,used,pico_ui::TextRole::prose,pico_ui::Actions{{result.count?"Copy root":nullptr,result.count?"Store X":nullptr,nullptr,nullptr,"Back"}});
    if(!action||!result.count)return;double ordinal=1;
    if(result.count>1&&(!analysis_number("Root number to use",ordinal)||ordinal<1||ordinal>result.count||ordinal!=std::floor(ordinal)))return;
    double value=result.points[int(ordinal)-1].x;workbench_use(&value,1,action);
}
void workbench_symbolic()
{
    expression_g result=pico_workbench_isolate(workbench.single);if(!result){if(rt.error())engine_error();else notice("Symbolic isolation unsupported","No numerical fallback; try Solve near guess");return;}
    if(screen_result("workbench_ui.workbench_symbolic.value_result1",object_p(+result),"Isolate X","",true,"Store X"))
    {
        expression_g left,right;
        if(!result->split(object::ID_TestEQ,left,right)){notice("Symbolic result","No isolated right-hand expression");return;}
        symbol_g name=symbol::make("X");object_g value=object_p(+right);
        if(!name||!directory::store_here(name,value))engine_error();else notice("X expression stored","Exact right-hand expression; Ctrl+S persists");
    }
}
bool workbench_edit_equation(const char *title,char *target)
{
    char next[160];snprintf(next,sizeof next,"%s",target);
    if(!formula_entry(title,next,false,true,true))return false;
    cleaner temporary;expression_g residual=pico_workbench_residual(next);
    if(!residual){engine_error();return false;}snprintf(target,160,"%s",next);return true;
}
void workbench_single()
{
    for(;;)
    {
        static const char *const items[]={"Edit equation in X","Initial guess","Solve near guess","Search roots on interval","Symbolic isolate X","Plot equation residual","Back"};
        int action=screen_menu("workbench_ui.workbench_single.choose1","Single equation",items,7);if(action<0||action==6)return;
        if(action==0)workbench_edit_equation("Single equation in X",workbench.single);
        if(action==1)analysis_number("Initial X guess",workbench.guess);
        if(action==2)workbench_numerical(false);
        if(action==3)workbench_interval();
        if(action==4)workbench_symbolic();
        if(action==5)workbench_plot(false);
    }
}
void workbench_system()
{
    for(;;)
    {
        static const char *const items[]={"Choose 2 or 3 unknowns","Edit equation 1","Edit equation 2","Edit equation 3","Initial guesses","Solve system","Back"};
        int action=screen_menu("workbench_ui.workbench_system.choose1","System: X,Y or X,Y,Z",items,7);if(action<0||action==6)return;
        if(action==0){static const char *const sizes[]={"Two: X and Y","Three: X, Y and Z","Back"};int choice=screen_menu("workbench_ui.workbench_system.choose2","System size",sizes,3);if(choice>=0&&choice<2)workbench.count=choice+2;}
        if(action>=1&&action<=3){if(action==3&&workbench.count==2)notice("Two-variable system","Select three unknowns to use equation 3");else workbench_edit_equation("System equation",workbench.equations[action-1]);}
        if(action==4)
        {
            double guesses[3];memcpy(guesses,workbench.guesses,sizeof guesses);bool ok=true;
            for(unsigned i=0;i<workbench.count&&ok;++i){char title[40];snprintf(title,sizeof title,"Initial %c guess",'X'+i);ok=analysis_number(title,guesses[i]);}
            if(ok)memcpy(workbench.guesses,guesses,sizeof guesses);
        }
        if(action==5)workbench_numerical(true);
    }
}
void workbench_parameters()
{
    for(;;)
    {
        static const char *const items[]={"Plot with parameters","Edit formula in X,A,B,C","Configure A","Configure B","Configure C","Plot window","Store a parameter","Back"};
        int action=screen_menu("workbench_ui.workbench_parameters.choose1","Parameter plot",items,8);if(action<0||action==7)return;
        if(action==0)workbench_plot(true);
        if(action==1)workbench_edit_equation("Formula in X,A,B,C",workbench.scene.curve[0]);
        if(action>=2&&action<=4)
        {
            auto next=workbench.parameters[action-2];
            if(!analysis_number("Minimum",next.minimum)||!analysis_number("Maximum",next.maximum)||!analysis_number("Step (positive)",next.step)||!analysis_number("Current value",next.value))continue;
            if(!pico_workbench::valid_parameter(next)){notice("Parameter","Require min < max, positive step, value in range");continue;}
            workbench.parameters[action-2]=next;
        }
        if(action==5)
        {
            static const char *const titles[]={"X minimum","X maximum","Y minimum","Y maximum"};double next[4];memcpy(next,workbench.scene.window,sizeof next);bool ok=true;
            for(int i=0;i<4&&ok;++i)ok=analysis_number(titles[i],next[i]);
            if(ok&&pico_graph_scene::bounds(next))memcpy(workbench.scene.window,next,sizeof next);else if(ok)notice("Window","Use ordered finite bounds");
        }
        if(action==6)
        {
            static const char *const names[]={"A","B","C","Back"};int choice=screen_menu("workbench_ui.workbench_parameters.choose2","Store parameter",names,4);if(choice<0||choice==3)continue;
            symbol_g name=symbol::make(names[choice]);object_g value=hwdouble::make(workbench.parameters[choice].value);
            if(!name||!value||!directory::store_here(name,value))engine_error();else notice("Parameter stored","Ctrl+S saves calculator variables");
        }
    }
}
void workbench_saved()
{
    if(!sys_disk_ok()){notice("Workbenches","SD card unavailable");return;}pico_f_mkdir("workbenches");
    static const char *const actions[]={"Save workbench","Load workbench","Back"};int action=screen_menu("workbench_ui.workbench_saved.choose1","Saved workbenches",actions,3);if(action<0||action==2)return;
    if(action==0)
    {
        char name[40]="LAST";if(!prompt("Workbench name",name,sizeof name,true))return;
        char path[120];snprintf(path,sizeof path,"workbenches/%s.workbench",name);if(file_exists(path)&&!confirm("Replace saved workbench?"))return;
        workbench.scene.angle=angle_number();
        bool ok=pico_workbench_save(path,workbench);if(ok)workbench_revision.saved();notice(ok?"Workbench saved":"Workbench save failed",ok?"Stored under /db48x/workbenches":"Previous file retained");return;
    }
    int &selected=pico_ui::focus("files.workbenches").selected;
    for(;;)
    {
        char names[8][256]={};int total=0,offset=selected/8*8;DIR directory;FILINFO info;
        FRESULT result=pico_f_opendir(&directory,"workbenches");if(result!=FR_OK){notice("Workbenches","Cannot read directory");return;}
        while((result=f_readdir(&directory,&info))==FR_OK&&*info.fname)
        {
            if(!file_visible(info))continue;char name[256];snprintf(name,sizeof name,"%s",info.fname);char *ext=strrchr(name,'.');if(!ext)continue;
            if((!strcasecmp(ext,".bak")||!strcasecmp(ext,".rollback"))){bool old_backup=!strcasecmp(ext,".bak");*ext=0;ext=strrchr(name,'.');if(!ext||strcasecmp(ext,".workbench"))continue;char path[320];snprintf(path,sizeof path,"workbenches/%s",name);if(file_exists(path))continue;if(old_backup){char pending[340];snprintf(pending,sizeof pending,"%s.rollback",path);if(file_exists(pending))continue;}}
            else if(strcasecmp(ext,".workbench"))continue;
            if(total>=offset&&total<offset+8)snprintf(names[total-offset],256,"%s",name);++total;
        }
        f_closedir(&directory);if(result!=FR_OK){notice("Workbenches","Directory read failed");return;}
        screen_page("workbench_ui.workbench_saved.page1","Load workbench",pico_ui::Actions{{total?"Open":nullptr,nullptr,nullptr,total?"Details":nullptr,"Back"}});for(int i=0;i<8&&i+offset<total;++i)file_row_label(i,names[i],i+offset==selected);list_page_hint(selected,total);if(!total)info_label(8,70,"None saved yet. Back returns to Save workbench.");flush_display();int key=read_key_raw();file_row_key(key,selected,total);
        if(key==27||key==0xb1||key==0x85)return;
        if(key==0xb5&&total)selected=(selected+total-1)%total;else if(key==0xb6&&total)selected=(selected+1)%total;
        else if(key==0xb4&&total)selected=std::max(0,selected-8);else if(key==0xb7&&total)selected=std::min(total-1,selected+8);
        else if(key==0x84&&total)screen_text("files.details","File details",names[selected-offset],strlen(names[selected-offset]));
        else if((key==10||key==13||key==0x81)&&total)
        {
            char path[320];snprintf(path,sizeof path,"workbenches/%s",names[selected-offset]);pico_workbench candidate;bool backup=false;pico_workbench_diagnostic diagnostic;
            bool ok=pico_workbench_load(path,candidate,backup,&diagnostic);
            if(!ok){char report[768];snprintf(report,sizeof report,"%s\nExpected %u bytes\n\nPrimary: open %d, size %lu, read %d, close %d\n%s\n\nBackup: open %d, size %lu, read %d, close %d\n%s\n\nCurrent workbench kept.",path,unsigned(pico_workbench::bytes),diagnostic.primary.open,(unsigned long)diagnostic.primary.bytes,diagnostic.primary.read,diagnostic.primary.close,diagnostic.primary_decode,diagnostic.backup.open,(unsigned long)diagnostic.backup.bytes,diagnostic.backup.read,diagnostic.backup.close,diagnostic.backup_decode);const char *message="The file and its backup could not be loaded. Your current workbench is unchanged.\n\nChoose Load again to select another file, or restore a known-good backup from SD.";for(;;){int action=screen_text("workbench_ui.workbench_saved.view_text1","Could not load workbench",message,strlen(message),pico_ui::TextRole::prose,pico_ui::Actions{{nullptr,nullptr,nullptr,"Details","Back"}});if(action!=4)break;screen_text("workbench_ui.load_details","Workbench load details",report,strlen(report));}return;}
            bool parsed=true;
            for(int i=0;i<5&&parsed;++i){cleaner temporary;expression_g eq=pico_workbench_residual(i==0?candidate.single:i<4?candidate.equations[i-1]:candidate.scene.curve[0]);parsed=bool(eq);}
            if(!parsed){engine_error();return;}
            if(!confirm("Load workbench and its angle mode?"))return;
            if(workbench_revision.dirty&&!confirm("Replace unsaved workbench changes?"))return;workbench=candidate;workbench_revision.saved();set_scene_angle(workbench.scene.angle);notice(backup?"Backup workbench loaded":"Workbench loaded","Open solver or parameter plot");return;
        }
    }
}
}
void pico_workbench_menu()
{
    if(rt.editing()){notice("Workbench","Finish or cancel the calculator entry");return;}
    save<bool> direct_screen(user_display_enable,false);workbench.scene.angle=angle_number();
    for(;;)
    {
        pico_ui::Mutation<pico_workbench> mutation(workbench,workbench_revision);
        static const char *const items[]={"Single equation","Equation system","Parameter plot","Saved workbenches","Help","Back"};char title[64];static const char *const angles[]={"Degrees","Radians","Grads","Pi radians"};snprintf(title,sizeof title,"Math workbench | %s",angles[angle_number()]);int action=screen_menu("workbench_ui.pico_workbench_menu.choose1",title,items,6);if(action<0||action==5)break;
        if(action==0)workbench_single();if(action==1)workbench_system();if(action==2)workbench_parameters();if(action==3)workbench_saved();
        if(action==4)
        {
            const char *text=pico_ui_help::tool_text("workbench_ui.pico_workbench_menu.view_text1");screen_text("workbench_ui.pico_workbench_menu.view_text1","Workbench help",text,strlen(text));
        }
    }
    key_pop_all();
}
