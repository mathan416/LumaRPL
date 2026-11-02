#pragma once
#include <stdint.h>
#include <stddef.h>
#include <ctime>
#include <cstring>
#include <cstdio>
struct pico_timezone
{
    enum rule_id {fixed=0,north_america=1,europe=2,australia=3,new_zealand=4,pacific_canada=5};
    int32_t minutes=-300;uint32_t rule=north_america; // Toronto default.
    bool valid()const{return minutes>=-840&&minutes<=840&&rule<=pacific_canada&&(!rule||minutes<=780);}
    static bool leap(int year){return year%4==0&&(year%100!=0||year%400==0);}
    static int days_in_month(int year,int month){const int days[]={31,28,31,30,31,30,31,31,30,31,30,31};return days[month-1]+(month==2&&leap(year));}
    static int64_t midnight(int year,int month,int day)
    {
        int64_t days=0;for(int y=1970;y<year;++y)days+=365+leap(y);for(int m=1;m<month;++m)days+=days_in_month(year,m);return (days+day-1)*86400;
    }
    static int sunday(int year,int month,int ordinal)
    {
        int day=ordinal<0?days_in_month(year,month):1;int weekday=int((midnight(year,month,day)/86400+4)%7);
        return ordinal<0?day-weekday:1+(7-weekday)%7+7*(ordinal-1);
    }
    int offset(int64_t utc)const
    {
        if(!rule)return minutes;
        if(rule==pacific_canada&&utc>=1772964000LL)return minutes+60;time_t time=time_t(utc);const tm *calendar=gmtime(&time);if(!calendar)return minutes;int year=calendar->tm_year+1900;
        int64_t start=0,end=0;bool southern=false;
        if(rule==north_america||rule==pacific_canada){start=midnight(year,3,sunday(year,3,2))+(120-minutes)*60;end=midnight(year,11,sunday(year,11,1))+(60-minutes)*60;}
        if(rule==europe){start=midnight(year,3,sunday(year,3,-1))+3600;end=midnight(year,10,sunday(year,10,-1))+3600;}
        if(rule==australia||rule==new_zealand)
        {
            int month=rule==australia?10:9;start=midnight(year,month,sunday(year,month,rule==australia?1:-1))+(120-minutes)*60;
            end=midnight(year,4,sunday(year,4,1))+(120-minutes)*60;southern=true;
        }
        bool summer=southern?(utc<end||utc>=start):(utc>=start&&utc<end);return minutes+(summer?60:0);
    }
    int64_t local(int64_t utc)const{return utc+int64_t(offset(utc))*60;}
    static bool fixed_offset(const char *text,int32_t &minutes)
    {
        if(strlen(text)!=6||(text[0]!='+'&&text[0]!='-')||text[3]!=':')return false;
        for(int i=1;i<6;++i)if(i!=3&&(text[i]<'0'||text[i]>'9'))return false;
        int hour=(text[1]-'0')*10+text[2]-'0',minute=(text[4]-'0')*10+text[5]-'0';if(hour>14||minute>59||(hour==14&&minute))return false;
        minutes=(text[0]=='-'?-1:1)*(hour*60+minute);return true;
    }
};
struct pico_timezone_choice {const char *name;int32_t minutes;uint32_t rule;};
static const pico_timezone_choice pico_timezones[]={
    {"Toronto / Eastern",-300,pico_timezone::north_america},{"UTC",0,pico_timezone::fixed},
    {"Vancouver / BC",-480,pico_timezone::pacific_canada},{"Denver / Mountain",-420,pico_timezone::north_america},
    {"Chicago / Central",-360,pico_timezone::north_america},{"Halifax / Atlantic",-240,pico_timezone::north_america},
    {"Newfoundland",-210,pico_timezone::north_america},{"London",0,pico_timezone::europe},{"Central Europe",60,pico_timezone::europe},
    {"Tokyo",540,pico_timezone::fixed},{"India",330,pico_timezone::fixed},{"Sydney",600,pico_timezone::australia},{"Auckland",720,pico_timezone::new_zealand}
};
inline void pico_timezone_name(const pico_timezone &zone,char *text,size_t capacity)
{
    for(const auto &entry:pico_timezones)if(entry.minutes==zone.minutes&&entry.rule==zone.rule){snprintf(text,capacity,"%s",entry.name);return;}
    int value=zone.minutes<0?-zone.minutes:zone.minutes;snprintf(text,capacity,"UTC%c%02d:%02d (fixed)",zone.minutes<0?'-':'+',value/60,value%60);
}
