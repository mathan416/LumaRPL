#include "http_response.h"
#include "currency_csv.h"
#include "network_time.h"
#include "tls_budget.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
static bool response(const std::string &wire,std::string &body,size_t step=1)
{
    char data[2048];pico_http_response parser(data,sizeof data);
    for(size_t i=0;i<wire.size();i+=step)if(!parser.feed(reinterpret_cast<const unsigned char *>(wire.data()+i),std::min(step,wire.size()-i)))return false;
    bool ok=parser.finish();if(ok)body=data;return ok;
}
int main()
{
    std::string body;assert(response("HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nabc",body)&&body=="abc");
    assert(response("HTTP/1.0 200 OK\r\n\r\nabc",body)&&body=="abc");
    for(int step:{1,2,17,256})assert(response("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n2\r\nab\r\n1\r\nc\r\n0\r\n\r\n",body,step)&&body=="abc");
    for(const char *wire:{"HTTP/1.1 500 Error\r\n\r\nno","HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\nabc","HTTP/1.1 200 OK\r\nContent-Length: 3\r\nContent-Length: 3\r\n\r\nabc","HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nContent-Length: 3\r\n\r\nabc","HTTP/1.1 200 OK\r\nContent-Encoding: gzip\r\n\r\nabc","HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\nzz\r\nabc\r\n0\r\n\r\n","HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n3\r\nab","HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n0\r\n"})assert(!response(wire,body));
    assert(!response(std::string("HTTP/1.0 200 OK\r\n\r\n")+std::string(2048,'x'),body));
    std::string nul="HTTP/1.0 200 OK\r\n\r\n";nul.append("a\0b",3);assert(!response(nul,body));
    std::ifstream file("tests/frankfurter_fixture.csv");assert(file);std::string csv((std::istreambuf_iterator<char>(file)),{});pico_currency_rates rates;assert(pico_currency_csv(csv.data(),csv.size(),rates)&&rates.count==30);
    uint8_t data[pico_currency_rates::bytes];assert(rates.encode(data,sizeof data)==sizeof data);pico_currency_rates original=rates;assert(!pico_currency_csv(csv.data(),csv.size(),rates,"2024-01-02")&&!memcmp(&rates,&original,sizeof rates));
    assert(!pico_currency_csv(csv.data(),csv.find_last_of('\n')-10,rates));assert(!memcmp(&rates,&original,sizeof rates));
    std::string wrong=csv;wrong.replace(wrong.find(",EUR,"),5,",USD,");assert(!pico_currency_csv(wrong.data(),wrong.size(),rates));
    int64_t epoch=0;assert(pico_network_utc("2026-10-31","12:34",epoch)&&epoch==1793450040);assert(!pico_network_utc("2026-02-30","12:00",epoch));assert(!pico_network_utc("2026-10-31","24:00",epoch));assert(!pico_network_utc("2038-01-01","00:00",epoch));
    assert(pico_tls_budget_reset());void *p=pico_tls_calloc(1,20000);assert(p&&pico_tls_used()>=20000);assert(!pico_tls_calloc(SIZE_MAX,4));assert(!pico_tls_calloc(1,41000));assert(!pico_tls_budget_reset());pico_tls_free(p);assert(!pico_tls_used()&&pico_tls_peak()>=20000);assert(pico_tls_budget_reset());
    puts("PASS: streaming HTTP lengths/chunks/truncation/bounds, real provider CSV, UTC and bounded TLS allocator");
}
