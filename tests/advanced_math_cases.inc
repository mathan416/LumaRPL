static void advanced_math_cases(){
 rt.reset();load_saved_keymap();if(!execute("42 'X' STO 5 'Y' STO 7 'Z' STO 123"))test_abort(__LINE__);auto angle=Settings.AngleMode();auto precision=Settings.Precision();auto dirs=rt.directories();pico_advanced_math d;
 lab_test_values(pico_polynomial_calculate(d,0),{3});lab_test_values(pico_polynomial_calculate(d,2,3),{8});lab_test_values(pico_polynomial_calculate(d,3),{3});lab_test_values(pico_polynomial_calculate(d,4),{3});lab_test_values(pico_polynomial_calculate(d,5),{0});
 {object_g roots=pico_polynomial_calculate(d,1);if(!roots||roots->type()!=object::ID_array)test_abort(__LINE__);unsigned n=0;for(object_p v:*array_p(+roots)){algebraic_g a=v->as_algebraic(),one=integer::make(1);algebraic_g residual=+(a*a-one);algebraic_g size=abs::evaluate(residual);double error;if(!size||!pico_lab_real(size,error)||error>1e-8)test_abort(__LINE__);++n;}if(n!=2)test_abort(__LINE__);}
 strcpy(d.coefficients[0][2],"1");{object_g roots=pico_polynomial_calculate(d,1);if(!roots)test_abort(__LINE__);unsigned n=0;for(object_p v:*array_p(+roots)){algebraic_g a=v->as_algebraic(),one=integer::make(1),r=+(a*a+one),size=abs::evaluate(r);double e;if(!size||!pico_lab_real(size,e)||e>1e-8)test_abort(__LINE__);++n;}if(n!=2)test_abort(__LINE__);}d=pico_advanced_math();
 {pico_system_evaluator e(d);double y[]={1.,0.},f[2];if(!e.valid()||!pico_system_evaluator::sample(&e,0,y,f)||f[0]!=0||f[1]!=-1)test_abort(__LINE__);}
 if(Settings.AngleMode()!=angle||Settings.Precision()!=precision||rt.directories()!=dirs||rt.depth()!=1||rt.top()->as_int32(0,false)!=123)test_abort(__LINE__);
 // Argument placement uses complete templates and preserves earlier undo history.
 for(auto language:{source_language::lua,source_language::assembly}){char text[512]="prefix ",clipboard[257]={};pico_text_editor e{text,sizeof text,strlen(text),strlen(text)};pico_editor_tools tools(e,clipboard);if(!tools.insert("!",1))test_abort(__LINE__);
 if(language==source_language::lua)force({0x84,'p','r','i','n','t',13,'1',0x81});else force({0x84,'L','D',13,'1',0x81});editor_insert_command(language,tools);if(language==source_language::lua){if(strcmp(text,"prefix !print(\"\")")||e.cursor!=15)test_abort(__LINE__);}else{if(strncmp(text+8,"LD ",3)||tools.first()!=11||tools.last()!=13)test_abort(__LINE__);}if(!tools.undo()||strcmp(text,"prefix !")||!tools.undo()||strcmp(text,"prefix "))test_abort(__LINE__);}
 mkdir("advanced",0700);d.dirty=true;if(!pico_math_save("advanced/TEST.advanced",d)||d.dirty)test_abort(__LINE__);pico_advanced_math restored;bool backup=false;if(!pico_math_load("advanced/TEST.advanced",restored,backup)||backup)test_abort(__LINE__);host_fail_save=true;d.frequency=2000;d.dirty=true;if(pico_math_save("advanced/TEST.advanced",d)||!d.dirty)test_abort(__LINE__);host_fail_save=false;if(!pico_math_load("advanced/TEST.advanced",restored,backup)||restored.frequency!=1000)test_abort(__LINE__);
 {double values[]={1,2};size_t before=rt.depth();force({0x85});advanced_values("advanced.statistics_result","Cancel","Unchanged",values,2);if(rt.depth()!=before)test_abort(__LINE__);force({0x81});advanced_values("advanced.statistics_result","Copy","[1,2]",values,2);if(rt.depth()!=before+1)test_abort(__LINE__);lab_test_values(rt.top(),{1,2});rt.drop();}
 for(int theme=0;theme<8;++theme){host_theme=theme;apply_theme(theme);
 {pico_document_access<pico_advanced_math> access(advanced_cache,advanced);if(!advanced)test_abort(__LINE__);*advanced=pico_advanced_math();
 host_variant="advanced-polynomial";force({'3','1',0x85,'3','2',13,0x85,0x85,0x85});polynomial_menu();
 host_variant="advanced-ode";force({'4',0x85,'5',0x85,'6',0x85,0x85});advanced_ode_menu();
 host_variant="advanced-ode-mode";force({'1',0x85,0x85});advanced_ode_menu();
 host_variant="advanced-engineering";force({'2',0x85,'3',0x85,'4',0x85,'5',0x85,0x85});engineering_menu();
 {pico_document_access<pico_data_table> access(data_cache,data_table);if(!data_table)test_abort(__LINE__);data_table->rows=5;for(int i=0;i<5;++i){data_table->points[i][0]=i+1;data_table->points[i][1]=i;}data_table->dirty=true;}
 host_variant="advanced-statistics";force({'3',0x85,'4',0x85,'5',0x85,0x85});inference_menu();
 host_variant="advanced-proportion";size_t depth=rt.depth();force({'6',13,13,0x81,0x85});inference_menu();if(rt.depth()!=depth+1)test_abort(__LINE__);lab_test_values(rt.top(),{.236593090512564,.763406909487436,0,1});rt.drop();
 host_variant="advanced-sample";force({'1',0x85,0x85});inference_menu();
 host_variant="advanced-files";force({0x85});advanced_files(0);
 for(unsigned mode=0;mode<4;++mode){host_variant="advanced-help-"+std::to_string(mode);force({0x85});advanced_help(mode);}
 advanced->dirty=false;}
 // Closing a result returns to its operation list, preserving the selected item.
 {pico_document_access<pico_math_lab> access(lab_cache,lab);if(!lab)test_abort(__LINE__);*lab=pico_math_lab();host_variant="back-vector-operation";force({'4','8',0x85,0x85,0x85});lab_vectors();if(forced_index!=5||pico_ui::focus("math.lab.vector_operations").selected!=7)test_abort(__LINE__);
 host_variant="back-probability-operation";force({'6','2',0x85,0x85,0x85});lab_probability();if(forced_index!=5||pico_ui::focus("math.lab.probability_operations").selected!=1)test_abort(__LINE__);lab->dirty=false;}
 }
 host_theme=0;apply_theme(0);host_variant="normal";puts("PASS: real-engine polynomial arithmetic and real/complex root residuals; system RHS and state isolation; argument caret/selection and undo; immediate-parent vector/probability Back and focus; four advanced workbenches/plots/help in eight themes");
}
