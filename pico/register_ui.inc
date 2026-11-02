struct matrix_register_page {int offset,total=0;object_g names[8],values[8];};
bool collect_matrix_register(object_p name,object_p value,void *opaque)
{
 if(value->type()!=object::ID_array)return true;array_g matrix=array_p(value);size_t rows,cols;if(!matrix->is_matrix(&rows,&cols,false))return true;
 auto &p=*static_cast<matrix_register_page *>(opaque);int n=p.total++-p.offset;if(n>=0&&n<8){p.names[n]=name;p.values[n]=value;}return true;
}
void store_matrix_register(const pico_matrix_document &doc)
{
 char name[13]="C";if(!prompt("Register name (e.g. C, RESULTS)",name,sizeof name))return;if(!pico_form_name(name)){engine_error();return;}
 symbol_g key=symbol::make(name);directory_g home=rt.homedir();if(!key||!home){engine_error();return;}if(home->recall(key)&&!confirm("Replace this HOME register?"))return;
 object_g value=pico_matrix_calculate(doc,0);if(!value||!const_cast<directory *>(+home)->store(object_g(object_p(+key)),value)){engine_error();return;}notice("Matrix register stored","Save calculator session to keep HOME registers after restart");
}
void matrix_registers()
{
 int &selected=pico_ui::focus("math.matrix.registers").selected;
 for(;;){matrix_register_page page{selected/8*8};rt.homedir()->enumerate(collect_matrix_register,&page);if(page.total&&selected>=page.total){selected=page.total-1;continue;}
 screen_page("math.matrix.registers","HOME matrix registers",{{page.total?"Use in A":nullptr,page.total?"Use in B":nullptr,"Store A","Actions","Back"}});
 for(int i=0;i<8&&i+page.offset<page.total;++i){char name[64]{};page.names[i]->render(name,sizeof name-1);label(8,38+i*28,name,selected==i+page.offset);}if(!page.total)info_label(8,70,"F3 stores A as a named HOME register");list_page_hint(selected,page.total);flush_display();int key=read_key_raw();file_row_key(key,selected,page.total);
 if(key==27||key==0xb1||key==0x85)return;if(page.total&&key==0xb5)selected=(selected+page.total-1)%page.total;if(page.total&&key==0xb6)selected=(selected+1)%page.total;if(page.total&&key==0xb4)selected=std::max(0,selected-8);if(page.total&&key==0xb7)selected=std::min(page.total-1,selected+8);
 if(key==0x83)store_matrix_register(*matrix_doc);
 if(page.total&&(key==0x81||key==13||key==10)){if(!matrix_doc->dirty||confirm("Replace unsaved matrix A from register?")){if(!pico_matrix_import(page.values[selected-page.offset],*matrix_doc))engine_error();}}
 if(page.total&&key==0x82){pico_document_access<pico_matrix_document> access(matrix_b_cache,matrix_b_doc);if(!matrix_b_doc){notice("Matrix B","Not enough memory");continue;}if(!matrix_b_doc->dirty||confirm("Replace unsaved matrix B from register?")){if(!pico_matrix_import(page.values[selected-page.offset],*matrix_b_doc))engine_error();}}
 if(key==0x84){const char *items[]={"View / copy selected register","Store matrix B","Register help"};int a=screen_menu("math.matrix.register_actions","Register actions",items,3);if(a==0&&!page.total)notice("No matrix selected","F3 stores A; Actions also offers Store B");if(a==0&&page.total)screen_result("math.matrix.result",page.values[selected-page.offset],"Matrix register","Stored in HOME; use Save session for persistence.");if(a==1){pico_document_access<pico_matrix_document> access(matrix_b_cache,matrix_b_doc);if(matrix_b_doc)store_matrix_register(*matrix_b_doc);else notice("Matrix B","Not enough memory");}if(a==2){const char *help=pico_ui_help::tool_text("math.matrix.register_help");screen_text("math.matrix.register_help","Matrix register help",help,strlen(help));}}
 }
}
void matrix_more()
{for(;;){
 const char *items[]={"Integer power A^n","Row echelon (REF)","Reduced row echelon (RREF)","Rank of A","Classify augmented system"};int action=screen_menu("math.matrix.reduction","Powers / row reduction",items,5);if(action<0)break;
 screen_page("math.matrix.busy","Calculating matrix",{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
 if(action==0){double n=2;if(!analysis_number("Integer exponent (-32 to 32)",n))continue;if(n< -32||n>32||n!=floor(n)){notice("Matrix power","Use an integer from -32 to 32");continue;}object_g value=pico_matrix_power(*matrix_doc,int(n));if(value)matrix_result(value,"Matrix power","Negative powers require invertible A; A^0 is identity.");else engine_error();continue;}
 pico_matrix_reduction info;object_g value=pico_matrix_reduce(*matrix_doc,action!=1,action==4,info);if(!value){engine_error();continue;}char text[200];snprintf(text,sizeof text,"Rank: %u\n%s",info.rank,action==4?(info.system==pico_matrix_reduction::unique?"Unique solution":info.system==pico_matrix_reduction::infinite?"Infinitely many solutions":"Inconsistent: no solution"):"Calculated by upstream DB48X elimination.");
 if(action==3){object_g rank=integer::make(info.rank);screen_result("math.matrix.result",rank,"Matrix rank",text);}else matrix_result(value,action==4?"Augmented-system RREF":items[action],text);
}
}
