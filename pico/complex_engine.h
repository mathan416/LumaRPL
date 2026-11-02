#pragma once
#include "exploration_documents.h"
#include "matrix_engine.h"
#include "conversion_engine.h"
inline algebraic_p pico_complex_component(const char *source)
{
 object_g raw=pico_form_run_expression(source);algebraic_g value=raw?raw->as_algebraic():nullptr;if(!value||unit::get(value)){rt.type_error();return nullptr;}
 algebraic_g check=value;{settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);if(!algebraic::to_decimal(check))return nullptr;}
 double number;if(!pico_conversion_real(check,number)){rt.type_error();return nullptr;}return value;
}
// Use the engine's angle API for tagged phases; physical-unit ratio
// conversion needlessly routes radians through an irrational unit definition.
inline bool pico_complex_angle(algebraic_g &value,object::id target)
{
 if(!unit::get(value))return true;
 settings::SaveHardwareFloatingPoint hardware(true);settings::SavePrecision precision(16);settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);
 auto source=algebraic::adjust_angle(value);if(source==object::ID_object){rt.dimension_error();return false;}
 value=algebraic::convert_angle(value,source,target,false,false);return value;
}
inline complex_p pico_complex_input(const pico_complex_document &doc,unsigned which,unit_g *captured=nullptr)
{
 settings::SaveHardwareFloatingPoint input_hardware(doc.polar[which]?true:Settings.HardwareFloatingPoint());settings::SavePrecision input_precision(doc.polar[which]?16:Settings.Precision());
 if(pico_quantity_affine(doc.input[which*2])||pico_quantity_affine(doc.input[which*2+1])){rt.dimension_error();return nullptr;}algebraic_g x=pico_quantity_parse(doc.input[which*2]),y=pico_quantity_parse(doc.input[which*2+1]);if(!x||!y)return nullptr;unit_g xu=unit::get(x),yu=unit::get(y),common=pico_form_unit_one(xu?+xu:doc.polar[which]?nullptr:+yu);
 if(doc.polar[which]){settings::SaveHardwareFloatingPoint angle_hardware(true);settings::SavePrecision angle_precision(16);settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);settings::SaveAutoSimplify angle_simplify(true);if(xu)x=xu->value();if(yu&&!pico_complex_angle(y,doc.radians?object::ID_Rad:object::ID_Deg))return nullptr;double magnitude;algebraic_g numeric=x;if(!algebraic::to_decimal(numeric)||!pico_conversion_real(numeric,magnitude)||magnitude<0){rt.value_error();return nullptr;}if(captured)*captured=+common;return polar::make(x,y,doc.radians?object::ID_Rad:object::ID_Deg);}
 if(common){algebraic_g *parts[]={&x,&y};for(auto p:parts){if(!unit::get(*p)){algebraic_g dimension=common->uexpr();*p=unit::make(*p,dimension);}if(!*p||!common->convert(*p))return nullptr;unit_g q=unit::get(*p);if(q)*p=q->value();}}
 if(captured)*captured=+common;return rectangular::make(x,y);
}
// Transcendental phase/root operations use the same bounded 16-digit numeric
// profile as the native graph tools. Exact rectangular arithmetic stays exact.
inline algebraic_p pico_complex_phase(complex_r value,object::id angle){settings::SaveHardwareFloatingPoint hardware(true);settings::SavePrecision precision(16);settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);return value->arg(angle);}
inline polar_p pico_complex_polar(complex_r value,object::id angle){algebraic_g magnitude=value->mod(),phase=pico_complex_phase(value,angle);settings::SaveHardwareFloatingPoint hardware(true);settings::SavePrecision precision(16);return magnitude&&phase?polar::make(magnitude,phase,angle):nullptr;}
struct pico_complex_roots {complex_g value;unsigned degree;object::id unit;};
inline object_p pico_complex_root_item(size_t,size_t,size_t row,size_t,void *opaque)
{
 settings::SaveHardwareFloatingPoint numeric_hardware(true);settings::SavePrecision numeric_precision(16);settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);auto &ctx=*static_cast<pico_complex_roots *>(opaque);algebraic_g magnitude=ctx.value->mod(),phase=ctx.value->arg(ctx.unit),n=integer::make(ctx.degree),one=integer::make(1),turn=integer::make(row*2);if(!magnitude||!phase||!n||!one||!turn){
return nullptr;}
 algebraic_g exponent=one/n;algebraic_g radius=exponent?pow::evaluate(magnitude,exponent):nullptr;
if(!radius)return nullptr;algebraic_g full;if(ctx.unit==object::ID_Deg)full=integer::make(180);else full=algebraic::pi();algebraic_g angle=(phase+turn*full)/n;polar_g root=polar::make(radius,angle,ctx.unit);if(!root){
return nullptr;}rectangular_g cart=root->as_rectangular();
return object_p(+cart);
}
// Operations: rectangular, polar, modulus, phase, conjugate, roots, +, -, *, /.
inline object_p pico_complex_calculate(const pico_complex_document &doc,unsigned action,unsigned degree=2)
{
 if(!doc.valid()||action>9||degree<2||degree>16){rt.value_error();return nullptr;}stack_depth_restore stack;object_g result;auto precision=Settings.Precision();
 {pico_workbench_scope scope;if(!scope.entered)return nullptr;settings::SavePrecision p(precision);settings::SaveAngleMode angle(doc.radians?object::ID_Rad:object::ID_Deg);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);settings::SaveHardwareFloatingPoint hardware(false);
 unit_g zunit; complex_g z=pico_complex_input(doc,0,&zunit);if(!z)return nullptr;
 if(action==0)result=object_p(+z->as_rectangular());if(action==1)result=object_p(pico_complex_polar(z,doc.radians?object::ID_Rad:object::ID_Deg));if(action==2)result=object_p(+z->mod());if(action==3){if(z->is_zero()){rt.value_error();return nullptr;}result=object_p(pico_complex_phase(z,doc.radians?object::ID_Rad:object::ID_Deg));}if(action==4)result=object_p(+z->conjugate());
 if(action==5){pico_complex_roots ctx{z,degree,doc.radians?object::ID_Rad:object::ID_Deg};result=array::build(degree,0,pico_complex_root_item,&ctx);
}
 if(action<6&&action!=3&&result&&zunit){algebraic_g dimension=zunit->uexpr();if(action==5){algebraic_g one=integer::make(1),degree_value=integer::make(degree),power=one/degree_value,unit_base=zunit->as_algebraic(),root_unit=pow::evaluate(unit_base,power);array_g roots=result->as<array>();if(!root_unit||!roots){
return nullptr;}result=roots->map(multiply::evaluate,root_unit);
}else{algebraic_g value=result->as_algebraic();if(!value)return nullptr;result=unit::make(value,dimension);}}
 if(action>=6){unit_g wunit;complex_g w=pico_complex_input(doc,1,&wunit);if(!w)return nullptr;algebraic_g a=zunit?algebraic_p(unit::make(z->as_algebraic(),zunit->uexpr())):algebraic_p(+z),b=wunit?algebraic_p(unit::make(w->as_algebraic(),wunit->uexpr())):algebraic_p(+w);if(!a||!b)return nullptr;if(action==6)result=object_p(+(a+b));if(action==7)result=object_p(+(a-b));if(action==8)result=object_p(+(a*b));if(action==9){if(w->is_zero()){rt.zero_divide_error();return nullptr;}result=object_p(+(a/b));}}
 if(result)result=rt.clone(result);
 }return result;
}
inline bool pico_complex_set_unit(pico_complex_document &doc,bool radians)
{
 if(doc.radians==radians)return true;pico_complex_document next=doc;auto precision=Settings.Precision();
 {pico_workbench_scope scope;if(!scope.entered)return false;settings::SavePrecision p(precision);settings::SaveNumericalResults exact(false);settings::SaveNumericalConstants constants(false);settings::SaveHardwareFloatingPoint hardware(false);
 for(unsigned which=0;which<2;++which)if(doc.polar[which]){algebraic_g angle=pico_quantity_parse(doc.input[which*2+1]);if(!angle)return false;unit_g explicit_unit=unit::get(angle);if(explicit_unit&&!pico_complex_angle(angle,doc.radians?object::ID_Rad:object::ID_Deg))return false;algebraic_g converted=angle?algebraic::convert_angle(angle,doc.radians?object::ID_Rad:object::ID_Deg,radians?object::ID_Rad:object::ID_Deg,false,false):nullptr;if(!converted)return false;if(!pico_formula_source(converted,next.input[which*2+1],32)){double value;if(!pico_conversion_real(converted,value)){if(!rt.error())rt.value_error();return false;}snprintf(next.input[which*2+1],32,"%.17g",value);}}
 }next.radians=radians;next.dirty=true;doc=next;return true;
}
