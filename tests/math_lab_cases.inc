static void lab_test_flat(object_p value,double *numbers,unsigned &count)
{
 if(!value||count>=16)test_abort(__LINE__);object_g rooted=value;
 if(value->type()==object::ID_array){for(object_p part:*array_p(value))lab_test_flat(part,numbers,count);return;}
 double v=0;if(value->type()==object::ID_expression){if(!pico_workbench_value(expression_p(value),v))test_abort(__LINE__);}else if(!pico_workbench_double(value->as_algebraic(),v)){char text[128]="";value->render(text,sizeof text);fprintf(stderr,"Flat conversion failed value=%s type=%u real=%d error=%s\n",text,unsigned(value->type()),int(value->is_real()),rt.error()?cstring(rt.error()):"none");test_abort(__LINE__);}numbers[count++]=v;
}
static void lab_test_values(object_p value,std::initializer_list<double> expected)
{
 static unsigned operation=0;++operation;if(!value){fprintf(stderr,"Lab operation %u failed: %s\n",operation,rt.error()?cstring(rt.error()):"no engine error");test_abort(__LINE__);}
 object_g rooted=value;pico_workbench_scope scope;if(!scope.entered||!scope.bind("X",2)||!scope.bind("Y",3))test_abort(__LINE__);double numbers[16];unsigned n=0;lab_test_flat(rooted,numbers,n);if(n!=expected.size())test_abort(__LINE__);unsigned i=0;for(double v:expected){if(fabs(numbers[i]-v)>1e-9*fmax(1.,fabs(v))){fprintf(stderr,"Lab value %.17g expected %.17g\n",numbers[i],v);test_abort(__LINE__);}++i;}
}
static void math_lab_cases()
{
 rt.reset();load_saved_keymap();if(!execute("42 'X' STO -7 'Y' STO 123"))test_abort(__LINE__);auto angle=Settings.AngleMode();auto precision=Settings.Precision();auto dirs=rt.directories();pico_math_lab doc;
 lab_test_values(pico_lab_vector(doc,2),{0});lab_test_values(pico_lab_vector(doc,3),{0,0,1});lab_test_values(pico_lab_vector(doc,4),{1});lab_test_values(pico_lab_vector(doc,5),{sqrt(2.)});lab_test_values(pico_lab_vector(doc,6),{0,0,0});lab_test_values(pico_lab_vector(doc,7),{90});lab_test_values(pico_lab_vector(doc,8),{1,1,0});lab_test_values(pico_lab_vector(doc,9),{1,-1,0});lab_test_values(pico_lab_vector(doc,10),{2,0,0});lab_test_values(pico_lab_vector(doc,11),{.5,0,0});lab_test_values(pico_lab_vector(doc,12),{1,0,0});
 doc.dimensions=2;lab_test_values(pico_lab_vector(doc,3),{1});strcpy(doc.vectors[0][0],"1/3");strcpy(doc.vectors[1][0],"1");strcpy(doc.vectors[1][1],"0");{object_g fraction=pico_lab_vector(doc,2);if(!fraction||!fraction->is_fraction())test_abort(__LINE__);lab_test_values(fraction,{1./3});}lab_test_values(pico_lab_vector(doc,6),{1./3,0});strcpy(doc.vectors[1][0],"0");if(pico_lab_vector(doc,6)||!rt.error())test_abort(__LINE__);rt.clear_error();doc=pico_math_lab();strcpy(doc.vectors[0][0],"1e100");strcpy(doc.vectors[0][1],"1e100");strcpy(doc.vectors[1][0],"1e100");strcpy(doc.vectors[1][1],"-1e100");lab_test_values(pico_lab_vector(doc,7),{90});doc=pico_math_lab();
 lab_test_values(pico_lab_calculus(doc,0),{4});lab_test_values(pico_lab_calculus(doc,1),{6});lab_test_values(pico_lab_calculus(doc,2),{4,6});lab_test_values(pico_lab_calculus(doc,3),{4,6,3,2});lab_test_values(pico_lab_calculus(doc,4),{2,0,0,2});double plane[3];if(!pico_lab_plane(doc,plane)||fabs(plane[0]-2)>1e-9||fabs(plane[1]-2)>1e-8||fabs(plane[2]-2)>1e-8)test_abort(__LINE__);pico_math_lab cusp=doc;strcpy(cusp.function,"ABS(X)+Y");cusp.x=0;if(pico_lab_plane(cusp,plane))test_abort(__LINE__);rt.clear_error();
 lab_test_values(pico_lab_algebra(doc,0),{9});lab_test_values(pico_lab_algebra(doc,1),{9});lab_test_values(pico_lab_algebra(doc,2),{9});lab_test_values(pico_lab_algebra(doc,4),{9});strcpy(doc.algebra,"1/(X^2-1)");lab_test_values(pico_lab_algebra(doc,3),{1./3});
 if(rt.depth()!=1||rt.top()->as_int32(0,false)!=123||Settings.AngleMode()!=angle||Settings.Precision()!=precision||rt.directories()!=dirs)test_abort(__LINE__);double binding;if(!pico_formula_scalar("X",binding)||binding!=42||!pico_formula_scalar("Y",binding)||binding!=-7)test_abort(__LINE__);
 mkdir("mathlabs",0700);doc=pico_math_lab();doc.dirty=true;if(!pico_math_save("mathlabs/TEST.mathlab",doc)||doc.dirty)test_abort(__LINE__);pico_math_lab loaded;bool backup=false;if(!pico_math_load("mathlabs/TEST.mathlab",loaded,backup)||backup)test_abort(__LINE__);doc.dimensions=2;doc.dirty=true;host_fail_save=true;if(pico_math_save("mathlabs/TEST.mathlab",doc)||!doc.dirty)test_abort(__LINE__);host_fail_save=false;if(!pico_math_load("mathlabs/TEST.mathlab",loaded,backup)||loaded.dimensions!=3)test_abort(__LINE__);
 {uint8_t bytes[pico_math_lab::bytes];loaded.encode(bytes);FILE *f=fopen("mathlabs/TEST.mathlab.bak","wb");fwrite(bytes,1,sizeof bytes,f);fclose(f);f=fopen("mathlabs/TEST.mathlab","wb");fputs("corrupt",f);fclose(f);if(!pico_math_load("mathlabs/TEST.mathlab",loaded,backup)||!backup||loaded.dimensions!=3)test_abort(__LINE__);remove("mathlabs/TEST.mathlab");remove("mathlabs/TEST.mathlab.bak");}
 for(int theme=0;theme<int(pico_themes::count);++theme){host_theme=theme;apply_theme(theme);{pico_document_access<pico_math_lab> access(lab_cache,lab);if(!lab)test_abort(__LINE__);*lab=pico_math_lab();
  host_variant="lab-vectors";force({0x85});lab_vectors();
  for(unsigned n=0;n<8;++n){host_variant=std::string("lab-vector-result-")+std::to_string(n);force({0x85});lab_result(pico_lab_vector(*lab,n),"Vector calculation");}
  host_variant="lab-vector-plot";force({'1',0x85});lab_vector_plot();
  host_variant="lab-probability";force({0x85});lab_probability();host_variant="lab-probability-copy";size_t old_depth=rt.depth();force({'6','2',0x81,0x85,0x85});lab_probability();double copied;if(rt.depth()!=old_depth+1||!pico_workbench_double(rt.top()->as_algebraic(),copied)||fabs(copied-.5)>1e-12)test_abort(__LINE__);rt.drop();
  for(int kind=0;kind<3;++kind){lab->probability.kind=kind;lab->probability.first=kind==0?0:kind==1?10:3;lab->probability.second=kind==1?.5:1;host_variant=std::string("lab-probability-plot-")+std::to_string(kind);force({0x85});lab_probability_plot();}
  lab->probability=pico_probability::distribution();host_variant="lab-regions";force({'5',0x85,0x85});lab_regions();
  host_variant="lab-strict-region";lab->relation[0]=2;force({'5',0x85,0x85});lab_regions();lab->relation[0]=1;
  host_variant="lab-multivariable";force({'4','3',0x85,0x85});lab_multivariable();
  host_variant="lab-double-integral";force({'4','7',0x85,0x85});lab_multivariable();
  host_variant="lab-surface";force({'6',0x85,0x85});lab_multivariable();host_variant="lab-tangent-plane";force({'7',0x85,0x85});lab_multivariable();
  host_variant="lab-algebra";force({'4','2',0x85,0x85});lab_algebra();
  for(unsigned n=0;n<5;++n){host_variant=std::string("lab-help-")+std::to_string(n);force({0xb7,0xb4,0x85});lab_help(n);}
  host_variant="lab-angle";force({'3',0x85});lab_files(0);
  host_variant="lab-vector-dimension";force({'3',0x85,0x85});lab_vectors();host_variant="lab-vector-products";force({'4',0x85,0x85});lab_vectors();host_variant="lab-vector-arithmetic";force({'5',0x85,0x85});lab_vectors();
  host_variant="lab-bounds";force({'5',0x85,0x85});lab_multivariable();host_variant="lab-variable";force({'2',0x85,0x85});lab_algebra();
  host_variant="lab-files";force({0x85});lab_files(0);host_sd=false;host_variant="lab-missing-sd";force({'1',0x85});lab_files(0);host_sd=true;
  lab->dirty=true;}
  host_variant="lab-home-routes";force({'2','5','2',0x85,0x85,0x85});pico_setup_menu();if(forced_index!=6)test_abort(__LINE__);force({'2','3','2',0x85,0x85,0x85});pico_setup_menu();if(forced_index!=6)test_abort(__LINE__);force({'2','3','3',0x85,0x85,0x85});pico_setup_menu();if(forced_index!=6)test_abort(__LINE__);force({'2','7',0x85,0x85,0x85});pico_setup_menu();if(forced_index!=5)test_abort(__LINE__);force({'2','3','4',0x85,0x85,0x85});pico_setup_menu();if(forced_index!=6)test_abort(__LINE__);
  host_variant="lab-exit-guard";force({0x85});if(pico_prepare_exit())test_abort(__LINE__);{pico_document_access<pico_math_lab> access(lab_cache,lab);if(!lab)test_abort(__LINE__);lab->dirty=false;}
 }
 host_variant="normal";host_theme=0;apply_theme(0);rt.reset();return_to_calculator=false;
 puts("PASS: real-engine vectors/exact fractions/projection/zero handling; symbolic gradient/Jacobian/Hessian/algebra; stack/settings/bindings preservation; CRC files/failed saves/backup; all five native workbenches/plots/help/exit guards in eight themes");
}
