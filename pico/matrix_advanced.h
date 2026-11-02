#pragma once
#include "matrix_engine.h"
inline bool pico_matrix_rank(array_r matrix,unsigned &rank)
{
 stack_depth_restore stack;size_t rows,cols;rank=0;if(!matrix||!matrix->is_matrix(&rows,&cols,true))return false;
 for(size_t r=0;r<rows;++r){bool nonzero=false;for(size_t c=0;c<cols;++c){object_g value=rt.pop();if(!value)return false;if(!value->is_zero(false))nonzero=true;}rank+=nonzero;}return true;
}
inline object_p pico_matrix_power(const pico_matrix_document &doc,int exponent)
{
 if(!doc.valid()||doc.rows!=doc.columns||exponent< -32||exponent>32){rt.dimension_error();return nullptr;}
 stack_depth_restore stack;object_g result;auto precision=Settings.Precision();
 {pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SavePrecision p(precision);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);settings::SaveHardwareFloatingPoint hardware(false);
 pico_matrix_build ctx{&doc};array_g base=array::build(doc.rows,doc.columns,pico_matrix_cell,&ctx),power=array::identity(doc.rows);if(!base||!power)return nullptr;
 if(exponent<0){base=base->invert();if(!base)return nullptr;exponent=-exponent;}
 for(unsigned n=unsigned(exponent);n;n>>=1){if(program::interrupted()){rt.interrupted_error();return nullptr;}if(n&1){power=array::mul(power,base);if(!power)return nullptr;}if(n>1){base=array::mul(base,base);if(!base)return nullptr;}}
 result=rt.clone(power);
 }return result;
}
struct pico_matrix_reduction {unsigned rank=0,coefficient_rank=0,unknowns=0;enum solution {not_system,unique,infinite,inconsistent} system=not_system;};
inline object_p pico_matrix_reduce(const pico_matrix_document &doc,bool reduced,bool augmented,pico_matrix_reduction &info)
{
 if(!doc.valid()||(augmented&&doc.columns<2)){rt.dimension_error();return nullptr;}
 stack_depth_restore stack;object_g result;auto precision=Settings.Precision();info={};
 {pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SavePrecision p(precision);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);settings::SaveHardwareFloatingPoint hardware(false);
 pico_matrix_build ctx{&doc};array_g matrix=array::build(doc.rows,doc.columns,pico_matrix_cell,&ctx);if(!matrix)return nullptr;
 echelon_options options;options.mode=reduced?echelon_mode::RREF:echelon_mode::REF;auto row=array::row_echelon(matrix,options);if(!row.matrix)return nullptr;if(!pico_matrix_rank(row.matrix,info.rank))return nullptr;
 if(augmented){array_g coefficients=array::build(doc.rows,doc.columns-1,pico_matrix_cell,&ctx);if(!coefficients)return nullptr;auto coeff=array::row_echelon(coefficients,options);if(!coeff.matrix)return nullptr;if(!pico_matrix_rank(coeff.matrix,info.coefficient_rank))return nullptr;info.unknowns=doc.columns-1;info.system=info.coefficient_rank<info.rank?pico_matrix_reduction::inconsistent:info.coefficient_rank<info.unknowns?pico_matrix_reduction::infinite:pico_matrix_reduction::unique;}
 result=rt.clone(row.matrix);
 }return result;
}
