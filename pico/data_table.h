#pragma once
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
struct pico_data_table
{
    static constexpr unsigned capacity=128;
    double points[capacity][2]{};unsigned rows=0,model=0;bool dirty=false;
    bool valid()const
    {
        if(rows>capacity||model>3)return false;
        for(unsigned i=0;i<rows;++i)for(double x:points[i])if(!std::isfinite(x)||std::fabs(x)>1e100)return false;
        return true;
    }
    void demo(){rows=6;model=0;for(unsigned i=0;i<rows;++i){points[i][0]=i;points[i][1]=2*i+1;}dirty=true;}
    void erase(unsigned row){if(row>=rows)return;memmove(points+row,points+row+1,(rows-row-1)*sizeof points[0]);--rows;dirty=true;}
    bool parse_csv(const char *source,size_t length)
    {
        if(length>16384||memchr(source,0,length))return false;pico_data_table next;
        size_t pos=0;if(length>=3&&!memcmp(source,"\xef\xbb\xbf",3))pos=3;bool first=true;
        while(pos<length)
        {
            char line[256];size_t start=pos;while(pos<length&&source[pos]!='\r'&&source[pos]!='\n')++pos;
            size_t n=pos-start;if(n>=sizeof line)return false;memcpy(line,source+start,n);line[n]=0;
            if(pos<length&&source[pos]=='\r')++pos;if(pos<length&&source[pos]=='\n')++pos;
            char *p=line;while(*p==' '||*p=='\t')++p;if(!*p)continue;
            if(first&&(!strcmp(p,"X,Y")||!strcmp(p,"x,y")||!strcmp(p,"\"X\",\"Y\""))){first=false;continue;}first=false;
            if(next.rows==capacity)return false;
            for(int c=0;c<2;++c)
            {
                while(*p==' '||*p=='\t')++p;bool quoted=*p=='"';if(quoted)++p;
                errno=0;char *end;double number=strtod(p,&end);if(end==p||errno==ERANGE||!std::isfinite(number)||std::fabs(number)>1e100)return false;
                p=end;while(*p==' '||*p=='\t')++p;
                if(quoted){if(*p!='"')return false;++p;while(*p==' '||*p=='\t')++p;}
                if(c==0){if(*p!=',')return false;++p;}else if(*p)return false;
                next.points[next.rows][c]=number;
            }
            ++next.rows;
        }
        next.dirty=false;*this=next;return true;
    }
    size_t csv(char *out,size_t capacity)const
    {
        if(!valid()||capacity<5)return 0;size_t used=4;memcpy(out,"X,Y\n",4);
        for(unsigned i=0;i<rows;++i){int n=snprintf(out+used,capacity-used,"%.17g,%.17g\n",points[i][0],points[i][1]);if(n<0||size_t(n)>=capacity-used)return 0;used+=n;}
        return used;
    }
};
