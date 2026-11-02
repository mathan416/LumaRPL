#include "currency_rates.h"
#include "input.h"
#include <cassert>
int main()
{
    assert(pico_translate_key(21)==PICO_CONVERSIONS);
    pico_currency_rates table;table.count=2;strcpy(table.rows[0].code,"EUR");strcpy(table.rows[0].date,"2026-10-30");strcpy(table.rows[0].rate,"1");strcpy(table.rows[1].code,"CAD");strcpy(table.rows[1].date,"2026-10-29");strcpy(table.rows[1].rate,"1.5");assert(table.valid());
    uint8_t data[pico_currency_rates::bytes];assert(table.encode(data,sizeof data)==sizeof data);pico_currency_rates copy;assert(copy.decode(data,sizeof data)&&copy.count==2&&!strcmp(copy.rows[1].rate,"1.5"));
    for(size_t i=0;i<sizeof data;++i){data[i]^=1;assert(!copy.decode(data,sizeof data));data[i]^=1;}assert(copy.count==2);assert(!copy.decode(data,sizeof data-1));
    assert(pico_currency_rates::date_valid("2000-02-29"));assert(!pico_currency_rates::date_valid("2100-02-29"));assert(!pico_currency_rates::date_valid("2026-02-30"));assert(!pico_currency_rates::rate_valid("nan"));assert(!pico_currency_rates::rate_valid("1e6"));assert(!pico_currency_rates::rate_valid("0"));assert(!pico_currency_rates::rate_valid("-1"));assert(pico_currency_rates::rate_valid("0.000000000001"));
    strcpy(table.rows[1].code,"EUR");assert(!table.valid());strcpy(table.rows[1].code,"CAD");memset(table.rows[1].rate,'1',24);assert(!table.valid());puts("PASS: currency wire format, every-byte corruption, semantic validation, dates and bounds");
}
