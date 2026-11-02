static void research_cases(){ {char body[1600];double value=.5;const char *units[]={"m"};if(!research_result_text(body,sizeof body,"Context retained",&value,1,units)||strncmp(body,"Value: 0.5 m",12)||!strstr(body,"Context retained"))test_abort(__LINE__);double values[]={1,-2,0};if(!research_result_text(body,sizeof body,"Copy X,Y,F",values,3)||!strstr(body,"1: 1")||!strstr(body,"2: -2")||!strstr(body,"3: 0"))test_abort(__LINE__);value=NAN;if(research_result_text(body,sizeof body,"",&value,1))test_abort(__LINE__);}rt.reset();load_saved_keymap();if(!execute("42 'X' STO 7 'A' STO 123"))test_abort(__LINE__);auto angle=Settings.AngleMode();auto precision=Settings.Precision();auto dirs=rt.directories();
 {pico_research_evaluator e("X^2+Y^2=25",true);double v;if(!e.valid()||!pico_research_evaluator::field(&e,3,4,v)||fabs(v)>1e-12)test_abort(__LINE__);}
 {double c[5];if(!pico_research_taylor("EXP(X)",0,4,c))test_abort(__LINE__);for(unsigned i=0,f=1;i<5;++i){if(fabs(c[i]-1./f)>1e-12)test_abort(__LINE__);f*=i+1;}}
 {pico_research_document d;d.correlation[0]=.5;pico_research_uncertainty_evaluator e(d);auto r=pico_research::correlated(pico_research_uncertainty_evaluator::evaluate,&e,d.measurement,d.sigma,d.correlation);if(!r.valid||fabs(r.nominal-6)>1e-9||fabs(r.sigma-sqrt(.37))>1e-6)test_abort(__LINE__);}
 if(rt.depth()!=1||rt.top()->as_int32(0,false)!=123||Settings.AngleMode()!=angle||Settings.Precision()!=precision||rt.directories()!=dirs)test_abort(__LINE__);
 {pico_complex_document d;strcpy(d.input[0],"3_m");strcpy(d.input[1],"400_cm");host_variant="research-complex-mod";science_test_quantity(pico_complex_calculate(d,2),"m",5);strcpy(d.input[2],"1_s");strcpy(d.input[3],"0_s");object_g product=pico_complex_calculate(d,8);if(!product||!unit::get(product->as_algebraic())){char text[200]={};if(product)product->render(text,sizeof text);fprintf(stderr,"Complex product: %s / %s\n",text,rt.error()?cstring(rt.error()):"none");test_abort(__LINE__);}object_g polar=pico_complex_calculate(d,1);if(!polar)test_abort(__LINE__);host_variant="research-complex-phase";science_test_quantity(pico_complex_calculate(d,3),"",53.13010235415598);strcpy(d.input[0],"3_m^2");strcpy(d.input[1],"4_m^2");object_g roots=pico_complex_calculate(d,5,2);if(!roots||roots->type()!=object::ID_array){fprintf(stderr,"Unitful roots: %s\n",rt.error()?cstring(rt.error()):"none");test_abort(__LINE__);}for(object_p root:*array_p(+roots))if(!unit::get(root->as_algebraic())){char text[100]={};root->render(text,sizeof text);fprintf(stderr,"Root lost unit: %s\n",text);test_abort(__LINE__);}}
 {pico_complex_document p;p.polar[0]=1;strcpy(p.input[0],"2_m");strcpy(p.input[1],"1.5707963267948966_r");host_variant="research-complex-explicit-angle";object_g phase=pico_complex_calculate(p,3);science_test_quantity(phase,"",90);}
 {host_variant="geometry-unit-assignment";force({'8','m',13,0x81,0x85});geometry_menu();{pico_document_access<pico_geometry_document> access(geometry_cache,geometry_doc);if(!geometry_doc||strcmp(geometry_doc->units,"m"))test_abort(__LINE__);}
 host_variant="geometry-unit-rescale";force({'8','c','m',13,0x85});geometry_menu();{pico_document_access<pico_geometry_document> access(geometry_cache,geometry_doc);if(!geometry_doc||strcmp(geometry_doc->units,"cm")||fabs(geometry_doc->points[0][0]+400)>1e-8)test_abort(__LINE__);*geometry_doc=pico_geometry_document();}}
 for(int theme=0;theme<8;++theme){host_theme=theme;apply_theme(theme);{pico_document_access<pico_research_document> access(research_cache,research);if(!research)test_abort(__LINE__);*research=pico_research_document();
 host_variant="research-home";force({0x85});research_launch(); // nested access resets pointer; reacquire below.
 }
 {pico_document_access<pico_research_document> access(research_cache,research);if(!research)test_abort(__LINE__);
 host_variant="research-contour";force({'4',13,0x85,0x82,'1',0x85,0x85});research_implicit();
 host_variant="research-optimization";size_t copied=rt.depth();force({'7','1',0x81,0x85});research_implicit();if(rt.depth()!=copied+1)test_abort(__LINE__);lab_test_values(rt.top(),{1,-2,0});rt.drop();
 host_variant="research-qr";force({'2','1',0x85,'2',0x85,0x85,0x85});research_linear();
 host_variant="research-svd";force({'3','1',0x85,'2',0x85,'3',0x85,0x85,0x85});research_linear();
 host_variant="research-pseudoinverse";force({'4',0x85,0x85});research_linear();
 host_variant="research-eigen";force({'5','1',0x85,'2',0x85,0x85,0x85});research_linear();
 host_variant="research-least-squares";force({'6',0x85,0x85});research_linear();
 host_variant="research-limit";force({'3','1',0x85,0x85});research_calculus();
 host_variant="research-taylor";force({'4','1',0x85,'2',0x85,0x85,0x85});research_calculus();
 research->first=1;research->last=3;strcpy(research->formula[2],"X^2");host_variant="research-sum";force({'5','1',0x85,0x85});research_calculus();research->first=0;strcpy(research->formula[2],"EXP(-X)");
 host_variant="research-improper";force({'6','1',0x85,0x85});research_calculus();
 host_variant="research-distribution";force({'2','1',0x85,'2','2',0x85,'2','3',0x85,0x85});research_statistics();
 host_variant="research-distribution-picker";force({'1',0x85,0x85});research_statistics();
 host_variant="research-binomial";size_t binomial_depth=rt.depth();force({'3',13,13,13,0x81,0x85});research_statistics();if(rt.depth()!=binomial_depth+1)test_abort(__LINE__);lab_test_values(rt.top(),{pico_research::exact_binomial(3,10,.5)});rt.drop();
 for(unsigned i=0;i<8;++i){research->data[0].points[i][0]=i;research->data[0].points[i][1]=2+3*i;}research->data[0].rows=8;
 host_variant="research-weighted-fit";force({'4',13,13,13,13,0x85,0x85});research_statistics();
 host_variant="research-welch-test";force({'5',0x85,0x85});research_statistics();
 host_variant="research-correlated";force({'4',0x85,0x85});research_uncertainty();
 host_variant="research-monte-carlo";force({'6',0x85,0x85});research_uncertainty();
 {pico_research_document demo;research->data[0]=demo.data[0];research->data[1]=demo.data[1];}
 host_variant="research-convolution";force({'2','1',0x85,0x85});research_signals();
 host_variant="research-psd";force({'3','1',0x85,0x85,0x85});research_signals();
 host_variant="research-poles";force({'5','1',0x85,0x85});research_signals();
 host_variant="research-bode";force({'6','1',0x85,0x85});research_signals();
 host_variant="research-response";force({'7','1',0x85,0x85});research_signals();
 host_variant="research-documents";force({0x85});research_documents();host_variant="research-files";force({0x85});research_files();
 host_variant="research-diagnostics";force({0x85});research_diagnostics();for(unsigned mode=0;mode<7;++mode){host_variant="research-help-"+std::to_string(mode);force({0x85});research_help(mode);}
 host_variant="research-codec";mkdir("studies",0700);research->dirty=true;if(!research_save("studies/TEST.study")||research->dirty)test_abort(__LINE__);double previous=research->fs;host_fail_save=true;research->fs=64;research->dirty=true;if(research_save("studies/TEST.study")||!research->dirty)test_abort(__LINE__);host_fail_save=false;bool backup=false;if(!research_read("studies/TEST.study",backup)||backup||research->fs!=previous)test_abort(__LINE__);
 host_sd=false;host_variant="research-missing-sd";force({'1',0x85,0x85});research_files();host_sd=true;research->dirty=true;}
 host_variant="research-exit-cancel";force({0x85});if(pico_prepare_exit())test_abort(__LINE__);{pico_document_access<pico_research_document> access(research_cache,research);if(!research||!research->dirty)test_abort(__LINE__);research->dirty=false;}
 }
 host_theme=0;apply_theme(0);host_variant="normal";puts("PASS: real-engine implicit residuals, Taylor derivatives and correlated unitful uncertainty; stack/settings/bindings preserved; six consolidated study controllers, native contours/plots, codecs/failed saves, help and diagnostics in eight themes");}
