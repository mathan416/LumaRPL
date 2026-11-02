#pragma once
#include "math_documents.h"
#include "form_engine.h"
#include "quantity_engine.h"
#include "array.h"
#include "functions.h"
struct pico_matrix_build {const pico_matrix_document *document;bool rhs=false;};
inline object_p pico_matrix_cell(size_t,size_t,size_t r,size_t c,void *opaque)
{
 auto &ctx=*static_cast<pico_matrix_build *>(opaque);object_g value=pico_form_run_expression(ctx.document->cells[r][ctx.rhs?ctx.document->columns-1:c]);
 algebraic_g a=value?value->as_algebraic():nullptr;if(!a||a->type()==object::ID_array||(!unit::get(a)&&a->is_symbolic())){if(!rt.error())rt.type_error();return nullptr;}return a;
}
inline object_p pico_matrix_calculate(const pico_matrix_document &doc,unsigned action)
{
 if(!doc.valid()){rt.value_error();return nullptr;}stack_depth_restore stack;object_g result;auto requested_precision=Settings.Precision();
 {pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SavePrecision precision(requested_precision);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);settings::SaveHardwareFloatingPoint hardware(false);
 pico_matrix_build ctx{&doc};bool solve=action==4;
 if((action==1||action==2)&&doc.rows!=doc.columns||solve&&doc.columns!=doc.rows+1||action==5&&(doc.rows!=2||doc.columns!=2)){rt.dimension_error();return nullptr;}
 array_g matrix=array::build(doc.rows,solve?doc.columns-1:doc.columns,pico_matrix_cell,&ctx);if(!matrix)return nullptr;
 if(action==0)result=object_p(+matrix);else if(action==1)result=matrix->determinant();else if(action==2)result=matrix->invert();else if(action==3)result=matrix->transpose();
 else if(solve){ctx.rhs=true;array_g b=array::build(doc.rows,1,pico_matrix_cell,&ctx),inverse=matrix->invert();if(b&&inverse)result=array::mul(inverse,b);}
 else if(action==5){object_g aa=pico_form_run_expression(doc.cells[0][0]),bb=pico_form_run_expression(doc.cells[0][1]),cc=pico_form_run_expression(doc.cells[1][0]),dd=pico_form_run_expression(doc.cells[1][1]);if(!aa||!bb||!cc||!dd)return nullptr;algebraic_g a=aa->as_algebraic(),b=bb->as_algebraic(),c=cc->as_algebraic(),d=dd->as_algebraic();algebraic_g two=integer::make(2),four=integer::make(4),delta=a-d,trace=a+d;algebraic_g disc=delta*delta+four*b*c;algebraic_g root=disc?sqrt::evaluate(disc):nullptr;if(root){algebraic_g x=(trace+root)/two,y=(trace-root)/two;if(x&&y)result=list::make(x,y);}}
 if(result)result=rt.clone(result);
 }return result;
}
// Division is explicitly A * inverse(B), preserving operand order.
inline object_p pico_matrix_arithmetic(const pico_matrix_document &left,const pico_matrix_document &right,unsigned operation,const char *scalar="1")
{
    if(!left.valid()||!right.valid()||operation>5){rt.value_error();return nullptr;}
    if((operation<2&&(left.rows!=right.rows||left.columns!=right.columns))||
       (operation==2&&left.columns!=right.rows)||
       (operation==3&&(right.rows!=right.columns||left.columns!=right.rows)))
    {rt.dimension_error();return nullptr;}
    stack_depth_restore stack;object_g result;auto requested_precision=Settings.Precision();
    {
        pico_workbench_scope scope;if(!scope.entered)return nullptr;
        settings::SavePrecision precision(requested_precision);
        settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);
        settings::SaveHardwareFloatingPoint hardware(false);
        pico_matrix_build a{&left},b{&right};
        array_g x=array::build(left.rows,left.columns,pico_matrix_cell,&a);
        if(!x)return nullptr;
        if(operation<4)
        {
            array_g y=array::build(right.rows,right.columns,pico_matrix_cell,&b);if(!y)return nullptr;
            if(operation<2)result=array::add_sub(x,y,operation==1);
            else if(operation==2)result=array::mul(x,y);
            else {array_g inverse=y->invert();if(inverse)result=array::mul(x,inverse);}
        }
        else
        {
            object_g input=pico_form_run_expression(scalar);algebraic_g k=input?input->as_algebraic():nullptr;
            if(!k){if(!rt.error())rt.type_error();return nullptr;}unit_g ku=unit::get(k);algebraic_g magnitude=ku?ku->value():+k;
            if(!k||!magnitude||!magnitude->is_real()||magnitude->is_symbolic()){if(!rt.error())rt.type_error();return nullptr;}
            if(operation==5&&k->is_zero(false)){rt.zero_divide_error();return nullptr;}
            result=x->map(operation==4?multiply::evaluate:divide::evaluate,k);
        }
        if(result)result=rt.clone(result);
    }
    return result;
}
inline bool pico_matrix_import(object_p input,pico_matrix_document &document)
{
 stack_depth_restore stack;object_g value=input;
 if(!value||value->type()!=object::ID_array){rt.type_error();return false;}
 array_g matrix=array_p(+value);size_t rows,columns;
 if(!matrix->is_matrix(&rows,&columns,true)||rows<1||rows>6||columns<1||columns>6){rt.dimension_error();return false;}
 pico_matrix_document next;next.reset(rows,columns);
 for(size_t i=rows*columns;i>0;--i){object_g cell=rt.pop();text_g text=cell?cell->as_text(true):nullptr;if(!text)return false;size_t length;utf8 source=text->value(&length);if(length>=32){rt.value_error();return false;}char *out=next.cells[(i-1)/columns][(i-1)%columns];memcpy(out,source,length);out[length]=0;}
 object_g checked=pico_matrix_calculate(next,0);if(!checked)return false;document=next;return true;
}
inline algebraic_p pico_result_approximation(object_p value)
{
 stack_depth_restore stack;object_g rooted=value;algebraic_g result;
 auto precision_value=Settings.Precision();
 {
  pico_workbench_scope scope;if(!scope.entered)return nullptr;
  settings::SavePrecision precision(precision_value);settings::SaveHardwareFloatingPoint hardware(false);settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);
  algebraic_g approx=rooted?rooted->as_algebraic():nullptr;
  if(approx&&algebraic::to_decimal(approx)&&!approx->is_symbolic()){object_g copy=rt.clone(approx);if(copy)result=copy->as_algebraic();}
 }
 return result;
}
