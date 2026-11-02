#pragma once
#include "conversion_engine.h"
#include "unit_catalog.h"
inline void run_port17_engine_tests()
{
    size_t depth=rt.depth(),directories=rt.directories();auto precision=Settings.Precision();bool hardware=Settings.HardwareFloatingPoint();
    auto magnitude=[](object_p result){unit_g units=result?unit::get(result):nullptr;algebraic_g value=units?units->value():result?result->as_algebraic():nullptr;double d=0;surface_check(pico_conversion_real(value,d),"conversion returns real magnitude");return d;};
    for(const auto &category:conversion_categories)for(int i=0;i<category.count;++i)
    {
        cleaner temporary;object_g converted=pico_convert_units("1",category.units[i].symbol,category.units[0].symbol);
        if(!converted)fprintf(stderr,"Unit catalog failure: %s -> %s\n",category.units[i].symbol,category.units[0].symbol);
        surface_check(converted,"all catalog units convert within their category");
    }
    object_g result=pico_convert_units("100","cm","m");surface_check(result&&fabs(magnitude(result)-1)<1e-12,"100 cm -> 1 m");rt.gc();surface_check(result&&fabs(magnitude(result)-1)<1e-12,"conversion result survives GC");
    result=pico_convert_units("1/3","m","cm");surface_check(result&&unit::get(result)->value()->is_fraction(),"conversion preserves exact fractions");
    result=pico_convert_units("0","°C","°F");surface_check(result&&fabs(magnitude(result)-32)<1e-9,"Celsius absolute offset");
    result=pico_convert_units("212","°F","°C");surface_check(result&&fabs(magnitude(result)-100)<1e-9,"Fahrenheit reverse offset");
    result=pico_convert_units("-40","°C","°F");surface_check(result&&fabs(magnitude(result)+40)<1e-9,"negative temperature");
    result=pico_convert_units("36","km/h","m/s");surface_check(result&&fabs(magnitude(result)-10)<1e-9,"compound speed");
    result=pico_convert_units("1","gal","l");surface_check(result&&fabs(magnitude(result)-3.785411784)<1e-9,"US liquid gallon");
    result=pico_convert_units("1","galUK","l");surface_check(result&&fabs(magnitude(result)-4.546092)<1e-9,"upstream imperial gallon");
    result=pico_convert_units("1","m","s");surface_check(!result,"incompatible dimensions fail");rt.clear_error();
    result=pico_convert_units("SQRT(-1)","m","ft");surface_check(!result,"complex input rejected");rt.clear_error();
    result=pico_convert_units("1/0","m","ft");surface_check(!result,"input domain failure");rt.clear_error();
    for(unsigned digits:{7u,16u,34u}){settings::SavePrecision selected(digits);settings::SaveNumericalResults numerical(true);result=pico_convert_units("100","cm","m");surface_check(result&&fabs(magnitude(result)-1)<1e-12&&Settings.Precision()==digits&&Settings.NumericalResults(),"unit conversion honors and restores caller settings");}
    {pico_workbench_scope fixture;surface_check(fixture.entered&&fixture.bind("VALUE",99),"conversion variable fixture");result=pico_convert_units("VALUE","cm","m");surface_check(result&&fabs(magnitude(result)-.99)<1e-12,"conversion can read scoped caller variables");symbol_g name=symbol::make("VALUE");object_g original=directory::recall_all(name,false);surface_check(original&&original->as_int32(0,false)==99,"conversion preserves caller variable");}
    key_pop_all();key_push(33);key_pop();key_push(33);program::count_interrupted=33;result=pico_convert_units("100","cm","m");surface_check(!result,"unit conversion cancellation");key_pop_all();rt.clear_error();surface_check(!program::halted,"conversion cancellation restores halted state");
    pico_currency_rates rates;rates.count=3;const char *codes[]={"EUR","USD","CAD"},*values[]={"1","1.25","1.5"};
    for(unsigned i=0;i<3;++i){strcpy(rates.rows[i].code,codes[i]);strcpy(rates.rows[i].date,"2026-10-30");strcpy(rates.rows[i].rate,values[i]);}
    result=pico_convert_currency("100",rates,1,2);surface_check(result&&fabs(magnitude(result)-120)<1e-12,"currency cross rate");
    result=pico_convert_currency("100",rates,2,1);surface_check(result&&fabs(magnitude(result)-100/1.2)<1e-10,"currency inverse");
    result=pico_convert_currency("1/3",rates,1,1);surface_check(result&&result->is_fraction(),"same currency exact fraction");
    result=pico_convert_currency("-10",rates,0,2);surface_check(result&&fabs(magnitude(result)+15)<1e-12,"negative currency amount");
    result=pico_convert_currency("1",rates,3,0);surface_check(!result,"invalid currency selection");rt.clear_error();
    surface_check(rt.depth()==depth&&rt.directories()==directories&&Settings.Precision()==precision&&Settings.HardwareFloatingPoint()==hardware,"conversions preserve stack, directory and settings");
    printf("PASS: port17 real-engine units, affine temperatures, exact fractions, compound units, currency crosses and preservation\n");
}
