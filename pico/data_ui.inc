namespace {
pico_data_table *data_table=nullptr;
pico_document_cache<pico_data_table> data_cache;
bool data_get()
{

    if(!data_table){notice("Data","Not enough bounded heap memory");return false;}return true;
}
void data_summary();
void data_plot(bool,unsigned * =nullptr);
void data_edit()
{
    unsigned row=0,column=0;
    for(;;)
    {
        char title[64];snprintf(title,sizeof title,"Data: %u rows%s",data_table->rows,data_table->dirty?" *":"");screen_page("data_ui.data_edit.page1",title,pico_ui::Actions{{"Add",data_table->rows?"Delete":nullptr,data_table->rows?"Stats":nullptr,data_table->rows?"Plot":nullptr,"Back"}});
        unsigned offset=row/8*8;
        for(unsigned i=offset;i<data_table->rows&&i<offset+8;++i)
        {
            char line[88];snprintf(line,sizeof line,"%u %cX=%.7g  %cY=%.7g",i+1,row==i&&column==0?'>':' ',data_table->points[i][0],row==i&&column==1?'>':' ',data_table->points[i][1]);label(4,38+(i-offset)*27,line,i==row);
        }
        if(!data_table->rows)info_label(8,70,"F1 adds the first row");ui_line(8,272,304,13,"Arrows: cell | Enter: edit",pico_ui::TextRole::status,pico_ui::text_colour());flush_display();int key=read_key_raw();
        if(key==27||key==0xb1||key==0x85)return;
        if(key==0xb5&&data_table->rows)row=(row+data_table->rows-1)%data_table->rows;
        else if(key==0xb6&&data_table->rows)row=(row+1)%data_table->rows;
        else if(key==0xb4||key==0xb7)column=1-column;
        else if(key==10||key==13){if(data_table->rows){double value=data_table->points[row][column];if(analysis_number(column?"Y value":"X value",value)){data_table->points[row][column]=value;data_table->dirty=true;}}}
        else if(key==0x81)
        {
            if(data_table->rows==pico_data_table::capacity){notice("Data","128-row capacity reached");continue;}
            double x=0,y=0;if(analysis_number("New X",x)&&analysis_number("New Y",y)){row=data_table->rows++;data_table->points[row][0]=x;data_table->points[row][1]=y;data_table->dirty=true;}
        }
        else if(key==0x82&&data_table->rows&&confirm("Delete selected row?")){data_table->erase(row);if(row&&row>=data_table->rows)--row;}
        else if(key==0x83)data_summary();
        else if(key==0x84)data_plot(false,&row);
    }
}
void data_summary()
{
    pico_data_summary result;screen_page("data_ui.data_summary.page1","Statistics",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
    if(!pico_data_describe(*data_table,result)){if(rt.error())engine_error();else notice("Statistics","Add finite data first");return;}
    static const char *const names[]={"Minimum","Maximum","Mean","Median","Sum","Population variance","Sample variance","Sample standard deviation"};
    char text[1700];size_t used=snprintf(text,sizeof text,"Rows: %u\n",data_table->rows);
    for(int c=0;c<2;++c){used+=snprintf(text+used,sizeof text-used,"\nColumn %c\n",'X'+c);for(int i=0;i<8;++i){if(i>=6&&!result.sample[c])used+=snprintf(text+used,sizeof text-used,"%s: unavailable (n<2)\n",names[i]);else used+=snprintf(text+used,sizeof text-used,"%s: %.12g\n",names[i],result.values[c][i]);}}
    screen_text("data_ui.data_summary.view_text1","Data summary",text,used);
    static const char *const actions[]={"Back","Copy statistic to stack"};if(screen_menu("data_ui.data_summary.choose1","Use statistics",actions,2)!=1)return;
    static const char *const columns[]={"X","Y","Back"};int column=screen_menu("data_ui.data_summary.choose2","Statistic column",columns,3);if(column<0||column==2)return;
    int metric=screen_menu("data_ui.data_summary.choose3","Copy which statistic?",names,8);if(metric<0)return;
    if(metric>=6&&!result.sample[column]){notice("Statistic unavailable","Sample variance/deviation needs at least two rows");return;}
    object_g value=hwdouble::make(result.values[column][metric]);if(!value||!rt.push(value))engine_error();
}
struct data_plot_context {pico_data_fit fit;bool fitted=false,residual=false;};
int data_plot_sample(void *opaque,int,double x,double,double *y)
{
    auto &context=*static_cast<data_plot_context *>(opaque);if(context.residual){*y=0;return 1;}
    if(!context.fitted)return 0;return pico_data_predict(context.fit,x,*y)?1:0;
}
void data_plot(bool residual,unsigned *selection)
{
    if(!data_table->rows){notice("Data plot","Add rows first");return;}
    data_plot_context context;context.residual=residual;context.fitted=pico_data_regress(*data_table,context.fit);
    if(!context.fitted){if(rt.error()&&(!strcmp(cstring(rt.error()),"Interrupted")||!strcmp(cstring(rt.error()),"Out of memory"))){engine_error();return;}rt.clear_error();if(residual){notice("Residuals","No valid fit; check model domain and X variation");return;}}
    pico_data_table *points=data_table,*temporary=nullptr;
    if(residual)
    {
        temporary=new(std::nothrow) pico_data_table(*data_table);if(!temporary){notice("Residuals","Not enough bounded heap memory");return;}points=temporary;
        for(unsigned i=0;i<points->rows;++i){double prediction;if(!pico_data_predict(context.fit,points->points[i][0],prediction)){delete temporary;notice("Residuals","Undefined fitted value");return;}points->points[i][1]-=prediction;}
    }
    pico_plot_request request{};request.active[0]=1;request.colour[0]=0x3080ef;
    snprintf(request.label[0],80,"%s",residual?"Residual baseline Y=0":context.fitted?"Fitted curve":"Scatter only (no valid fit)");
    if(!pico_data_window(*points,request.window)){delete temporary;notice("Data plot","Invalid data bounds");return;}
    plot_source source{data_plot_sample,&context,nullptr,points};source.scatter_selection=selection;plot_curves(request,source);delete temporary;
}
void data_regression()
{
    static const char *const models[]={"Linear: a+bX","Logarithmic: a+b ln X","Exponential: a exp(bX)","Power: a X^b","Back"};int selected=screen_menu("data_ui.data_regression.choose1","Regression model",models,5);if(selected<0||selected==4)return;
    data_table->model=selected;pico_data_fit fit;screen_page("data_ui.data_regression.page1","Fitting data",pico_ui::Actions{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
    if(!pico_data_regress(*data_table,fit)){if(rt.error())engine_error();else notice("Regression","Need valid data and varying X");return;}
    char text[640];snprintf(text,sizeof text,"%s\na (intercept/scale)=%.12g\nb (slope/exponent)=%.12g\n%s\nFitted correlation%s: %.10g\nR squared: %.10g\n",models[selected],fit.intercept,fit.slope,selected==1||selected==3?"X must be positive.":selected==2?"Y must be positive.":"",fit.correlation_valid?(selected?" (transformed)":""):" unavailable",fit.correlation_valid?fit.correlation:0,fit.correlation_valid?fit.correlation*fit.correlation:0);
    if(!fit.correlation_valid){char *at=strstr(text,"Fitted correlation");if(at)snprintf(at,sizeof text-size_t(at-text),"Correlation and R squared unavailable (e.g. constant Y).\n");}
    screen_text("data_ui.data_regression.view_text1","Regression",text,strlen(text));
    static const char *const options[]={"Copy coefficient a","Copy coefficient b","Copy fitted expression","Use as Y1 (replace 2D curves)","Save curve as graph scene","Copy correlation","Copy R squared","Back"};
    int action=screen_menu("data_ui.data_regression.choose2","Use fitted curve",options,8);if(action<0||action==7)return;
    if(action==0||action==1||action==5||action==6)
    {
        if(action>=5&&!fit.correlation_valid){notice("Fit statistic","Correlation is unavailable for this data");return;}
        double number=action==0?fit.intercept:action==1?fit.slope:action==5?fit.correlation:fit.correlation*fit.correlation;
        object_g value=hwdouble::make(number);if(!value||!rt.push(value))engine_error();return;
    }
    pico_graph_scene scene=graph_scene;scene.angle=angle_number();
    if(!pico_data_curve_scene(*data_table,fit.model,fit.intercept,fit.slope,scene)){notice("Fitted curve","Unable to construct a bounded curve");return;}
    if(action==2)
    {
        expression_g expression=pico_formula_parse(scene.curve[0]);object_g value=object_p(+expression);
        if(!value||!rt.push(value))engine_error();return;
    }
    if(action==3){if(graph_revision.dirty&&!confirm("Replace unsaved graph changes?"))return;graph_scene=scene;graph_revision.dirty=true;notice("Fitted curve ready","Ctrl+G > 2D functions > Plot / trace curves");return;}
    if(action==4)
    {
        if(!sys_disk_ok()){notice("Graph scene","SD card unavailable");return;}pico_f_mkdir("graphs");char path[320];
        if(!document_save_name("graphs",".scene",path,sizeof path))return;
        uint8_t data[760];size_t n=scene.encode(data,sizeof data);bool ok=n&&save_bytes(path,data,n);
        notice(ok?"Fitted scene saved":"Scene save failed",ok?"Ctrl+G > Saved graphs":"Previous file retained");
    }
}
void data_files()
{
    if(!sys_disk_ok()){notice("Data files","SD card unavailable");return;}pico_f_mkdir("datasets");
    static const char *const items[]={"Save / export CSV","Load / import CSV","Back"};int action=screen_menu("data_ui.data_files.choose1","Data files",items,3);if(action<0||action==2)return;char path[320];
    if(action==0){if(!document_save_name("datasets",".csv",path,sizeof path))return;bool ok=pico_data_save(path,*data_table);if(ok)data_table->dirty=false;notice(ok?"CSV saved":"CSV save failed",ok?"Stored under /db48x/datasets":"Previous file retained");}
    else if(document_file("datasets",".csv",path,sizeof path))
    {
        if(data_table->dirty&&!confirm("Replace unsaved table?"))return;bool backup=false;
        bool ok=pico_data_load(path,*data_table,backup);notice(ok?(backup?"CSV backup loaded":"CSV loaded"):"CSV load failed",ok?"X,Y table ready":"Current table kept; use <=128 finite XY rows");
    }
}
}
void pico_data_menu()
{
    if(rt.editing()){notice("Data","Finish or cancel the calculator entry");return;}bool first=!data_cache.resident&&!data_cache.handle;pico_document_access<pico_data_table> access(data_cache,data_table);if(first&&data_table)data_table->demo();if(!data_get())return;save<bool> direct_screen(user_display_enable,false);
    for(;;)
    {
        static const char *const items[]={"Edit X,Y table","Summary statistics","Regression model / result","Scatter + fitted curve","Residual scatter","CSV files","Reset demo","Back"};int action=screen_menu("data_ui.pico_data_menu.choose1","Data / statistics",items,8);if(action<0||action==7)break;
        if(action==0)data_edit();if(action==1)data_summary();if(action==2)data_regression();if(action==3)data_plot(false);if(action==4)data_plot(true);if(action==5)data_files();if(action==6&&confirm("Replace table with demo?"))data_table->demo();
    }
    key_pop_all();
}
