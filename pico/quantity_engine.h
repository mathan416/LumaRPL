#pragma once
#include "form_engine.h"
inline bool pico_quantity_affine(const char *s){return strstr(s,"°C")||strstr(s,"°F");}
inline algebraic_p pico_quantity_parse(const char *source){
    char text[192];size_t n=strnlen(source,sizeof text);if(n>=sizeof text){rt.value_error();return nullptr;}
    // Accept the common Ohm spelling as well as DB48X's canonical ohm / Ω.
    size_t at=0;for(size_t i=0;i<n;){if(i+3<=n&&!strncmp(source+i,"Ohm",3)){memcpy(text+at,"ohm",3);at+=3;i+=3;}else text[at++]=source[i++];}text[at]=0;
    object_g result=pico_form_run_expression(text);algebraic_g value=result?result->as_algebraic():nullptr;if(!value){if(!rt.error())rt.type_error();return nullptr;}
    unit_g u=unit::get(value);algebraic_g magnitude=u?u->value():+value;double finite;
    {settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);if(!algebraic::to_decimal(magnitude)||!pico_workbench_double(magnitude,finite)){if(!rt.error())rt.type_error();return nullptr;}}
    return value;
}
inline algebraic_p pico_quantity_number(double number,const char *declaration){
    algebraic_g value=hwdouble::make(number);if(!value)return nullptr;if(!*declaration)return value;
    unit_g target=pico_form_unit(declaration);if(!target)return nullptr;algebraic_g dimension=target->uexpr();return unit::make(value,dimension);
}
inline bool pico_quantity_value(algebraic_g value,const char *declaration,double &number,bool implicit=true){
    if(!value)return false;unit_g u=unit::get(value);
    if(*declaration){unit_g target=pico_form_unit(declaration);if(!target)return false;
        if(!u){if(!implicit&&!value->is_zero()){rt.dimension_error();return false;}algebraic_g dimension=target->uexpr();value=unit::make(value,dimension);}
        if(!value||!target->convert(value))return false;u=unit::get(value);
    }else if(u){rt.dimension_error();return false;}
    algebraic_g magnitude=u?u->value():+value;settings::SaveNumericalResults numerical(true);settings::SaveNumericalConstants constants(true);return algebraic::to_decimal(magnitude)&&pico_workbench_double(magnitude,number);
}
inline bool pico_quantity_input(const char *source,const char *declaration,double &number,bool implicit=true){algebraic_g value=pico_quantity_parse(source);return value&&pico_quantity_value(value,declaration,number,implicit);}
inline bool pico_quantity_bind(const char *name,double value,const char *units){symbol_g key=symbol::make(name);algebraic_g quantity=pico_quantity_number(value,units);return key&&quantity&&directory::store_here(key,quantity);}
inline bool pico_quantity_unit_name(algebraic_g value,char *out,size_t capacity){unit_g u=unit::get(value);if(!u){*out=0;return true;}size_t n=u->uexpr()->render(out,capacity);if(n>=capacity)return false;n=strlen(out);if(n>=2&&out[0]=='\''&&out[n-1]=='\''){memmove(out,out+1,n-2);out[n-2]=0;}return true;}
