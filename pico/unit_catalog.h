#pragma once
struct conversion_unit {const char *label,*symbol;};
struct conversion_category {const char *name;const conversion_unit *units;int count;};
const conversion_unit length_units[]={{"Metres","m"},{"Centimetres","cm"},{"Millimetres","mm"},{"Kilometres","km"},{"Inches","in"},{"Feet","ft"},{"Yards","yd"},{"Miles","mi"}};
const conversion_unit mass_units[]={{"Kilograms","kg"},{"Grams","g"},{"Milligrams","mg"},{"Pounds","lb"},{"Ounces","oz"}};
const conversion_unit temperature_units[]={{"Celsius (absolute)","°C"},{"Fahrenheit (absolute)","°F"},{"Kelvin","K"}};
const conversion_unit area_units[]={{"Square metres","m^2"},{"Square centimetres","cm^2"},{"Square kilometres","km^2"},{"Square feet","ft^2"},{"Acres","acre"},{"Hectares","ha"}};
const conversion_unit volume_units[]={{"Litres","l"},{"Millilitres","ml"},{"Cubic metres","m^3"},{"US liquid gallons","gal"},{"Imperial gallons","galUK"}};
const conversion_unit speed_units[]={{"Metres / second","m/s"},{"Kilometres / hour","km/h"},{"Miles / hour","mi/h"},{"Feet / second","ft/s"}};
const conversion_unit time_units[]={{"Seconds","s"},{"Minutes","min"},{"Hours","h"},{"Days","d"}};
const conversion_unit pressure_units[]={{"Pascals","Pa"},{"Kilopascals","kPa"},{"Bar","bar"},{"Atmospheres","atm"},{"Pounds / square inch","psi"}};
const conversion_unit energy_units[]={{"Joules","J"},{"Kilojoules","kJ"},{"Watt hours","W*h"},{"Kilowatt hours","kW*h"}};
const conversion_unit power_units[]={{"Watts","W"},{"Kilowatts","kW"},{"Horsepower","hp"}};
const conversion_unit angle_units[]={{"Degrees","°"},{"Radians","r"},{"Grads","grad"}};
#define CONVERSION_CATEGORY(name,array) {name,array,int(sizeof array/sizeof *array)}
const conversion_category conversion_categories[]={CONVERSION_CATEGORY("Length",length_units),CONVERSION_CATEGORY("Mass",mass_units),CONVERSION_CATEGORY("Temperature",temperature_units),CONVERSION_CATEGORY("Area",area_units),CONVERSION_CATEGORY("Volume",volume_units),CONVERSION_CATEGORY("Speed",speed_units),CONVERSION_CATEGORY("Time",time_units),CONVERSION_CATEGORY("Pressure",pressure_units),CONVERSION_CATEGORY("Energy",energy_units),CONVERSION_CATEGORY("Power",power_units),CONVERSION_CATEGORY("Angle",angle_units)};
#undef CONVERSION_CATEGORY
