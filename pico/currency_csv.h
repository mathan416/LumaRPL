#pragma once
#include "currency_rates.h"
inline bool pico_currency_csv(const char *text,size_t length,pico_currency_rates &rates,const char *as_of=nullptr)
{
    if(as_of&&*as_of&&!pico_currency_rates::date_valid(as_of))return false;
    if(!length||length>2047||memchr(text,0,length))return false;pico_currency_rates next;strcpy(next.provider,"Frankfurter (daily reference)");next.count=1;strcpy(next.rows[0].code,"EUR");strcpy(next.rows[0].rate,"1");
    size_t at=0;unsigned lines=0;const char *expected="AED AUD BRL CAD CHF CNY CZK DKK GBP HKD HUF IDR ILS INR JPY KRW MXN MYR NOK NZD PHP PLN SAR SEK SGD THB TRY USD ZAR";
    while(at<length)
    {
        size_t start=at;while(at<length&&text[at]!='\n')++at;size_t count=at-start;if(at<length)++at;if(count&&text[start+count-1]=='\r')--count;
        if(!count||count>=96)return false;char line[96];memcpy(line,text+start,count);line[count]=0;
        if(!lines++){if(strcmp(line,"date,base,quote,rate"))return false;continue;}
        if(next.count>=next.maximum)return false;char *date=line,*base=strchr(date,',');if(!base)return false;*base++=0;char *code=strchr(base,',');if(!code)return false;*code++=0;char *rate=strchr(code,',');if(!rate)return false;*rate++=0;
        if(strcmp(base,"EUR")||strlen(code)!=3||strlen(date)!=10||strlen(rate)>=24||strchr(rate,','))return false;
        bool known=false;for(const char *p=expected;*p;p+=4){if(!memcmp(p,code,3)){known=true;break;}if(!p[3])break;}
        if(!known)return false;auto &row=next.rows[next.count++];strcpy(row.code,code);strcpy(row.date,date);strcpy(row.rate,rate);
        if(!*next.rows[0].date||strcmp(row.date,next.rows[0].date)<0)strcpy(next.rows[0].date,row.date);
    }
    // Refuse partial results as a successful table refresh.
    if(next.count!=30||!next.valid())return false;
    if(as_of&&*as_of)for(unsigned i=0;i<next.count;++i)if(strcmp(next.rows[i].date,as_of)>0)return false;
    rates=next;return true;
}
