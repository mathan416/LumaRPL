static void matrix_arithmetic_cases()
{
 stack_depth_restore stack;auto precision=Settings.Precision();auto angle=Settings.AngleMode();size_t depth=rt.depth(),directories=rt.directories();
 pico_matrix_document a,b;a.reset(2,2);b.reset(2,2);strcpy(a.cells[0][0],"1");strcpy(a.cells[0][1],"2");strcpy(a.cells[1][0],"3");strcpy(a.cells[1][1],"4");strcpy(b.cells[0][0],"2");strcpy(b.cells[1][1],"3");
 auto check=[&](object_p input,const double *expected,size_t rows,size_t cols){object_g result=input;port201_check(result&&result->type()==object::ID_array,"arithmetic result type");array_g matrix=array_p(+result);size_t r,c;port201_check(matrix->is_matrix(&r,&c,true)&&r==rows&&c==cols,"arithmetic dimensions");for(size_t i=r*c;i>0;--i){object_g item=rt.pop();double value;port201_check(item&&pico_conversion_real(item->as_algebraic(),value)&&fabs(value-expected[i-1])<1e-13,"arithmetic result values");}};
 double sum[]={3,2,3,7},difference[]={-1,2,3,1},product[]={2,6,6,12},quotient[]={.5,2.0/3,1.5,4.0/3},scaled[]={1.0/3,2.0/3,1,4.0/3},divided[]={3,6,9,12};
 check(pico_matrix_arithmetic(a,b,0),sum,2,2);check(pico_matrix_arithmetic(a,b,1),difference,2,2);check(pico_matrix_arithmetic(a,b,2),product,2,2);check(pico_matrix_arithmetic(a,b,3),quotient,2,2);check(pico_matrix_arithmetic(a,b,4,"1/3"),scaled,2,2);check(pico_matrix_arithmetic(a,b,5,"1/3"),divided,2,2);
 b.reset(3,1);port201_check(!pico_matrix_arithmetic(a,b,0)&&rt.error(),"addition dimension error");rt.clear_error();port201_check(!pico_matrix_arithmetic(a,b,2)&&rt.error(),"multiply dimension error");rt.clear_error();
 a.reset(2,3);strcpy(a.cells[0][0],"1");strcpy(a.cells[0][1],"2");strcpy(a.cells[0][2],"3");strcpy(a.cells[1][0],"4");strcpy(a.cells[1][1],"5");strcpy(a.cells[1][2],"6");for(unsigned r=0;r<3;++r)strcpy(b.cells[r][0],"1");double rectangular[]={6,15};check(pico_matrix_arithmetic(a,b,2),rectangular,2,1);
 a.reset(2,2);b.reset(2,2);strcpy(b.cells[0][0],"0");strcpy(b.cells[1][1],"0");port201_check(!pico_matrix_arithmetic(a,b,3)&&rt.error(),"singular division");rt.clear_error();port201_check(!pico_matrix_arithmetic(a,b,5,"0")&&rt.error(),"zero scalar division");rt.clear_error();port201_check(!pico_matrix_arithmetic(a,b,4,"UNDEFINED_WORD")&&rt.error(),"invalid scalar");rt.clear_error();
 port201_check(rt.depth()==depth&&rt.directories()==directories&&Settings.Precision()==precision&&Settings.AngleMode()==angle,"arithmetic restores calculator state");
 if(getenv("LUMARPL_MATRIX_NUMERIC_ONLY")){puts("PASS: instrumented production matrix arithmetic with real-engine values, dimensions, division order, scalar errors and state preservation; renderer not exercised");return;}
 for(unsigned theme=0;theme<8;++theme)
 {
  host_theme=theme;apply_theme(theme);pico_document_access<pico_matrix_document> left(matrix_cache,matrix_doc);port201_check(matrix_doc,"operand allocation");matrix_doc->reset(2,2);strcpy(matrix_doc->cells[0][0],"1/3");matrix_doc->dirty=false;{pico_document_access<pico_matrix_document> right(matrix_b_cache,matrix_b_doc);port201_check(matrix_b_doc,"B allocation");matrix_b_doc->reset(2,2);matrix_b_doc->dirty=false;}
  host_variant="arithmetic-b";force({'1',0x85,0x85});matrix_arithmetic();
  host_variant="arithmetic-b-files";force({'2',0x85,0x85});matrix_arithmetic();
  host_variant="arithmetic-add";force({'3',0x81,0x85,0x85});matrix_arithmetic();object_g copied=rt.pop();port201_check(copied&&copied->type()==object::ID_array,"UI addition copies matrix");
  host_variant="arithmetic-chain";force({'3',0x83,0x85,0x85});matrix_arithmetic();port201_check(!strcmp(matrix_doc->cells[0][0],"4/3")&&matrix_doc->dirty,"Use in A chains exact result");
  host_variant="arithmetic-cancel";char retained[32];strcpy(retained,matrix_doc->cells[0][0]);force({'3',0x83,0x85,0x85,0x85});matrix_arithmetic();port201_check(!strcmp(retained,matrix_doc->cells[0][0]),"cancel replacement preserves A");
  host_variant="arithmetic-scalar";force({'7','1','1','/','3',13,0x85,0x85});matrix_arithmetic();
  host_variant="arithmetic-swap";force({'8',0x85});matrix_arithmetic();{pico_document_access<pico_matrix_document> right(matrix_b_cache,matrix_b_doc);port201_check(!strcmp(matrix_b_doc->cells[0][0],"4/3")&&matrix_b_doc->dirty,"swap retains both inputs");
  mkdir("matrices",0700);port201_check(pico_math_save("matrices/MATRIXB.matrix",*matrix_b_doc),"save B");strcpy(matrix_b_doc->cells[0][0],"99");bool backup=false;port201_check(pico_math_load("matrices/MATRIXB.matrix",*matrix_b_doc,backup)&&!strcmp(matrix_b_doc->cells[0][0],"4/3"),"reload B");matrix_b_doc->dirty=true;}port201_check(pending_document(9)==1,"loader protects B");matrix_doc->dirty=false;{pico_document_access<pico_matrix_document> right(matrix_b_cache,matrix_b_doc);matrix_b_doc->dirty=false;}
 }
 host_variant="normal";puts("PASS: matrix arithmetic real-engine values, rectangular products, noncommuting division order, exact scalar fractions, errors, state preservation, A/B controls, chaining/cancel, swap and B persistence/exit protection in eight themes");
}
