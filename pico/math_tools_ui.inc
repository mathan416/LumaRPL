#include "math_document_file.h"
// Native bounded mathematical workbenches, shared by firmware and host tests.
namespace {
pico_matrix_document *matrix_doc=nullptr;pico_document_cache<pico_matrix_document> matrix_cache;
pico_matrix_document *matrix_b_doc=nullptr;pico_document_cache<pico_matrix_document> matrix_b_cache;
pico_finance_document *finance_doc=nullptr;pico_document_cache<pico_finance_document> finance_cache;
template<class T>bool math_file_save(T &document,const char *folder,const char *extension)
{
 if(!sys_disk_ok()){notice("Save unavailable","Insert an SD card; your work remains in memory");return false;}pico_f_mkdir(folder);char path[320];if(!document_save_name(folder,extension,path,sizeof path))return false;
 bool ok=pico_math_save(path,document);notice(ok?"Saved":"Save failed",ok?"Document saved on SD":"Previous valid file and working document retained");return ok;
}
template<class T>void math_file_load(T &document,const char *folder,const char *extension)
{
 if(!sys_disk_ok()){notice("Load unavailable","Insert an SD card");return;}pico_f_mkdir(folder);char path[320];if(!document_file(folder,extension,path,sizeof path))return;if(document.dirty&&!confirm("Replace unsaved document?"))return;
 bool backup=false;bool ok=pico_math_load(path,document,backup);notice(ok?(backup?"Backup loaded":"Loaded"):"Load failed",ok?"Document ready":"Invalid/unavailable file; current document retained");
}
void matrix_grid(const char *name="A")
{
 unsigned row=0,column=0;
 for(;;){char title[56];snprintf(title,sizeof title,"Matrix %s %ux%u%s",name,matrix_doc->rows,matrix_doc->columns,matrix_doc->dirty?" *":"");screen_page("math.matrix.grid",title,{{"Edit","Size","Result","Save","Back"}});unsigned first=column/3*3;
 for(unsigned c=first;c<matrix_doc->columns&&c<first+3;++c){char text[20];snprintf(text,sizeof text,"Col %u",c+1);ui_line(12+(c-first)*102,40,98,16,text,pico_ui::TextRole::status,pico_ui::text_colour());}
 for(unsigned r=0;r<matrix_doc->rows;++r)for(unsigned c=first;c<matrix_doc->columns&&c<first+3;++c){bool selected=r==row&&c==column;char text[38];snprintf(text,sizeof text,"%s%s",selected?"> ":"",matrix_doc->cells[r][c]);ui_line(8+(c-first)*102,66+r*28,98,26,text,pico_ui::TextRole::prose,pico_ui::text_colour());}
 char details[76];snprintf(details,sizeof details,"Cell (%u,%u) | Arrows select | Enter edit",row+1,column+1);ui_line(8,257,304,26,details,pico_ui::TextRole::status,pico_ui::text_colour());flush_display();int key=read_key_raw();
 if(key==27||key==0xb1||key==0x85)return;
 if(key==0xb5)row=(row+matrix_doc->rows-1)%matrix_doc->rows;if(key==0xb6)row=(row+1)%matrix_doc->rows;if(key==0xb4)column=(column+matrix_doc->columns-1)%matrix_doc->columns;if(key==0xb7)column=(column+1)%matrix_doc->columns;
 if(key==13||key==10||key==0x81){char text[32];strcpy(text,matrix_doc->cells[row][column]);char title[40];snprintf(title,sizeof title,"Cell (%u,%u), expression",row+1,column+1);if(prompt(title,text,sizeof text)){pico_matrix_document next=*matrix_doc;strcpy(next.cells[row][column],text);object_g checked=pico_matrix_calculate(next,0);if(!checked)engine_error();else{strcpy(matrix_doc->cells[row][column],text);matrix_doc->dirty=true;}}}
 if(key==0x82){double rows=matrix_doc->rows,cols=matrix_doc->columns;if(analysis_number("Rows (1-6)",rows)&&analysis_number("Columns (1-6)",cols)){if(rows<1||rows>6||cols<1||cols>6||rows!=floor(rows)||cols!=floor(cols))notice("Matrix size","Use whole dimensions from 1 to 6");else if(confirm("Replace matrix with an identity/zero grid?")){matrix_doc->reset(unsigned(rows),unsigned(cols));row=column=0;}}}
 if(key==0x83){object_g value=pico_matrix_calculate(*matrix_doc,0);screen_result("math.matrix.result",value,"Matrix","F1 copies the matrix; F4 selects approximate copying.");}
 if(key==0x84)math_file_save(*matrix_doc,"matrices",".matrix");
 }
}
void matrix_files(pico_matrix_document &document,const char *name)
{
    char title[40];snprintf(title,sizeof title,"Matrix %s files",name);
    const char *files[]={"Save matrix","Load matrix","Import stack matrix"};
    int file=screen_menu("math.matrix.files",title,files,3);
    if(file==0)math_file_save(document,"matrices",".matrix");
    if(file==1)math_file_load(document,"matrices",".matrix");
    if(file==2){if(!rt.depth())notice("Stack empty","Put a matrix on the calculator stack first");else if(!document.dirty||confirm("Replace unsaved matrix from stack?")){if(!pico_matrix_import(rt.top(),document))engine_error();}}
}
void matrix_result(object_p input,const char *title,const char *context)
{
    object_g result=input;if(!result){engine_error();return;}
    while(screen_result("math.matrix.result",result,title,context,result->type()==object::ID_array,"Use in A"))
    {
        if(matrix_doc->dirty&&!confirm("Replace unsaved matrix A with this result?"))continue;
        // Import validates the complete result before replacing any grid cells.
        if(!pico_matrix_import(result,*matrix_doc)){engine_error();continue;}
        notice("Matrix A updated","The result is ready for the next calculation");return;
    }
}
void matrix_arithmetic()
{
    pico_document_access<pico_matrix_document> access(matrix_b_cache,matrix_b_doc);
    if(!matrix_b_doc){notice("Matrix B unavailable","Not enough bounded heap; matrix A retained");return;}
    for(;;)
    {
        char title[64];snprintf(title,sizeof title,"A %ux%u | B %ux%u",matrix_doc->rows,matrix_doc->columns,matrix_b_doc->rows,matrix_b_doc->columns);
        const char *items[]={"Edit matrix B","Matrix B files","A + B","A - B","A * B","A / B = A * inverse(B)","Scalar arithmetic","Swap A and B"};
        int action=screen_menu("math.matrix.arithmetic",title,items,8);if(action<0)return;
        if(action==0){save<pico_matrix_document *> selected(matrix_doc,matrix_b_doc);matrix_grid("B");}
        if(action==1)matrix_files(*matrix_b_doc,"B");
        if(action>=2&&action<=5)
        {
            screen_page("math.matrix.busy","Calculating matrices",{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
            object_g result=pico_matrix_arithmetic(*matrix_doc,*matrix_b_doc,action-2);
            if(!result)engine_error();else matrix_result(result,items[action],"Inputs A and B are retained. Copy to stack or Use in A explicitly.");
        }
        if(action==6)
        {for(;;){
            const char *items[]={"k * A","A / k","Negate A"};int choice=screen_menu("math.matrix.scalar","Scalar arithmetic",items,3);if(choice<0)break;
            char scalar[32]="2";if(choice==2)strcpy(scalar,"-1");else if(!prompt("Scalar k (e.g. 1/3)",scalar,sizeof scalar))continue;
            object_g result=pico_matrix_arithmetic(*matrix_doc,*matrix_b_doc,choice==1?5:4,scalar);
            if(!result)engine_error();else matrix_result(result,items[choice],"Exact scalar arithmetic. Copy to stack or Use in A explicitly.");
        }}
        if(action==7){pico_matrix_document temporary=*matrix_doc;*matrix_doc=*matrix_b_doc;*matrix_b_doc=temporary;matrix_doc->dirty=matrix_b_doc->dirty=true;}
    }
}
#include "register_ui.inc"
void matrix_menu()
{
    if(rt.editing()){notice("Matrix","Finish or cancel calculator entry");return;}
    pico_document_access<pico_matrix_document> access(matrix_cache,matrix_doc);
    if(!matrix_doc){notice("Matrix unavailable","Not enough bounded heap");return;}
    save<bool> display(user_display_enable,false);
    for(;;)
    {
        const char *names[]={"Edit matrix A","Matrix arithmetic (A, B)","Matrix operations","Matrix A files","Help","Named registers","Powers / row reduction"};
        int a=screen_menu("math.matrix.menu","Matrix workbench",names,7);if(a<0)return;
        if(a==0)matrix_grid();if(a==1)matrix_arithmetic();
        if(a==2)
        {for(;;){
            const char *operations[]={"Determinant","Inverse","Transpose","Solve augmented [A | b]","Eigenvalues (2x2)"};
            int operation=screen_menu("math.matrix.operations","Matrix A operations",operations,5);if(operation<0)break;
            screen_page("math.matrix.busy","Calculating matrix",{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
            object_g value=pico_matrix_calculate(*matrix_doc,operation+1);
            if(!value)engine_error();else matrix_result(value,operations[operation],operation==3?"Each result row is one unknown. Last input column is b.":"Copy explicitly; matrix results also offer Use in A.");
        }}
        if(a==3)matrix_files(*matrix_doc,"A");
        if(a==5)matrix_registers();if(a==6)matrix_more();
        if(a==4){const char *text=pico_ui_help::tool_text("math.matrix.help");screen_text("math.matrix.help","Matrix help",text,strlen(text));}
    }
}
void finance_inputs()
{
 for(;;){char text[8][70];snprintf(text[0],70,"Principal / initial value: %.10g",finance_doc->principal);snprintf(text[1],70,"Nominal annual rate (%%): %.10g",finance_doc->annual);snprintf(text[2],70,"Number of periods: %.10g",finance_doc->periods);snprintf(text[3],70,"Periods per year: %.10g",finance_doc->frequency);snprintf(text[4],70,"Payments: %s of period",finance_doc->beginning?"beginning":"end");snprintf(text[5],70,"NPV discount per period (%%): %.10g",finance_doc->discount);snprintf(text[6],70,"Payment for rate/term: %.10g",finance_doc->repayment);snprintf(text[7],70,"Deposit each period: %.10g",finance_doc->deposit);const char *names[]={text[0],text[1],text[2],text[3],text[4],text[5],text[6],text[7]};int a=screen_menu("math.finance.inputs","Finance inputs",names,8);if(a<0)return;pico_finance_document next=*finance_doc;if(a==4)next.beginning=!next.beginning;else{double *value=a==0?&next.principal:a==1?&next.annual:a==2?&next.periods:a==3?&next.frequency:a==5?&next.discount:a==6?&next.repayment:&next.deposit;if(!analysis_number(a==1?"Nominal annual rate (%)":a==5?"Discount per cash-flow period (%)":names[a],*value))continue;}if(!next.valid()){notice("Invalid finance input","Principal >=0; rates >-100%; periods 1-100000, frequency 1-366 (whole numbers)");continue;}next.dirty=true;*finance_doc=next;}
}
void finance_flows()
{
 for(;;){char text[17][64];const char *names[17];for(unsigned i=0;i<finance_doc->count;++i){snprintf(text[i],64,"CF%u: %.12g%s",i,finance_doc->flows[i],i?"":" (now)");names[i]=text[i];}int a=screen_menu("math.finance.flows","Signed cash flows",names,finance_doc->count,"Edit CF0 now, then equally spaced cash flows");if(a<0)return;double next=finance_doc->flows[a];if(analysis_number(text[a],next)){finance_doc->flows[a]=next;finance_doc->dirty=true;}}
}
bool finance_cancelled(){return sampling_poll(nullptr,-1);}
#include "finance_extended_ui.inc"
void finance_menu()
{
 if(rt.editing()){notice("Finance","Finish or cancel calculator entry");return;}pico_document_access<pico_finance_document> access(finance_cache,finance_doc);if(!finance_doc){notice("Finance unavailable","Not enough bounded heap");return;}save<bool> display(user_display_enable,false);
 for(;;){static const char *const names[]={"Finance inputs","Loan payment","Growth / rate / term","Amortization","Cash flows / NPV","Save finance","Load finance","Help"};int a=screen_menu("math.finance.menu","Financial calculator",names,8);if(a<0)return;
 if(a==0)finance_inputs();if(a==2)finance_more();if(a==1){double value;bool ok=a==1?pico_finance::payment(finance_doc->principal,finance_doc->annual,finance_doc->periods,finance_doc->frequency,finance_doc->beginning,value):pico_finance::growth(finance_doc->principal,finance_doc->annual,finance_doc->periods,finance_doc->frequency,value);if(!ok){notice("Finance calculation failed","Check inputs or reduce magnitude; result is outside numeric range");continue;}char detail[220];snprintf(detail,sizeof detail,"%.8g%% nominal/year; %.8g periods/year; %.8g periods. %s\nNo fees; no rounding of intermediate values.",finance_doc->annual,finance_doc->frequency,finance_doc->periods,a==1?(finance_doc->beginning?"Payment at beginning":"Payment at end"):"Compounds once per period");object_g result=decimal::from(value);screen_result("math.finance.result",result,names[a],detail);}
 if(a==3){double count=finance_doc->periods;if(!analysis_number("Payments made (whole number)",count))continue;if(count<0||count>finance_doc->periods||count!=floor(count)){notice("Amortization","Count must be 0 to the number of periods");continue;}screen_page("math.finance.busy","Amortizing payments",{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();double balance,interest,principal;if(!pico_finance::amortize(finance_doc->principal,finance_doc->annual,finance_doc->periods,finance_doc->frequency,finance_doc->beginning,unsigned(count),balance,interest,principal,finance_cancelled)){notice("Amortization","Cancelled or result outside numeric range; inputs retained");continue;}char text[250];snprintf(text,sizeof text,"After %.0f payments\nPrincipal repaid: %.12g\nInterest paid: %.12g\nNo intermediate cent rounding; excludes fees.",count,principal,interest);object_g value=decimal::from(balance);screen_result("math.finance.result",value,"Remaining balance",text);}
 if(a==4){static const char *const actions[]={"Edit signed cash flows","Set cash-flow count (1-17)","Calculate NPV","Set discount per period","Find IRR in interval","IRR search bounds"};for(;;){int choice=screen_menu("math.finance.cashmenu","Cash flows / NPV",actions,6);if(choice<0)break;if(choice==4)finance_irr();if(choice==5){double lo=finance_doc->irr_low,hi=finance_doc->irr_high;if(analysis_number("First IRR rate (%) per period",lo)&&analysis_number("Last IRR rate (%) per period",hi)){if(lo<=-100||hi>10000||lo>=hi)notice("IRR bounds","Use -100 < first < last <= 10000 (%)");else{finance_doc->irr_low=lo;finance_doc->irr_high=hi;finance_doc->dirty=true;}}}if(choice==0)finance_flows();if(choice==1){double count=finance_doc->count;if(analysis_number("Count includes CF0 (1-17)",count)){if(count>=1&&count<=17&&count==floor(count)){finance_doc->count=unsigned(count);finance_doc->dirty=true;}else notice("Cash-flow count","Use a whole number, 1 to 17");}}if(choice==2){double value;if(!pico_finance::npv(finance_doc->flows,finance_doc->count,finance_doc->discount,value)){notice("NPV unavailable","Check cash flows and discount rate");continue;}char text[180];snprintf(text,sizeof text,"Discount: %.10g%% per cash-flow period. CF0 occurs now; others at equal intervals. Negative amounts are outflows.",finance_doc->discount);object_g result=decimal::from(value);screen_result("math.finance.result",result,"Net present value",text);}if(choice==3){double rate=finance_doc->discount;if(analysis_number("Discount per period (%)",rate)){if(rate>-100&&rate<=10000){finance_doc->discount=rate;finance_doc->dirty=true;}else notice("Discount rate","Use >-100% and <=10000%");}}}}
 if(a==5)math_file_save(*finance_doc,"finance",".finance");if(a==6)math_file_load(*finance_doc,"finance",".finance");if(a==7){const char *text=pico_ui_help::tool_text("math.finance.help");screen_text("math.finance.help","Finance conventions",text,strlen(text));}
 }
}
}
