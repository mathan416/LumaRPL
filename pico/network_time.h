#pragma once
#include "currency_rates.h"
inline bool pico_network_utc(const char *date,const char *time,int64_t &epoch)
{
    if(!pico_currency_rates::date_valid(date)||strlen(time)!=5||time[2]!=':')return false;
    for(int i=0;i<5;++i)if(i!=2&&(time[i]<'0'||time[i]>'9'))return false;
    int year=atoi(date),month=atoi(date+5),day=atoi(date+8),hour=atoi(time),minute=atoi(time+3);if(year<2020||year>2037||hour>23||minute>59)return false;
    int days=0;for(int y=1970;y<year;++y)days+=365+(y%4==0&&(y%100!=0||y%400==0));const int months[]={31,28,31,30,31,30,31,31,30,31,30,31};for(int m=1;m<month;++m)days+=months[m-1]+(m==2&&year%4==0);days+=day-1;
    epoch=int64_t(days)*86400+hour*3600+minute*60;return true;
}
