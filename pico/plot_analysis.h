#pragma once
#include "plot_engine.h"
#include <cmath>
#include <cfloat>
namespace pico_plot_analysis {
enum class operation { roots,intersections,minimum,maximum,integral,area,derivative,between };
enum class status { ok,no_result,domain,not_converged,cancelled,callback_error,budget,invalid };
struct point {double x,y;};
struct result {status state=status::invalid;double value=0,error=0;point points[16]{};int count=0,evaluations=0;bool truncated=false;};
struct evaluator {
    pico_plot_sample sample;void *context;int curve,other;
    pico_plot_engine::poll_function poll=nullptr;void *poll_context=nullptr;
    int evaluations=0,limit=8192;status state=status::ok;bool difference=false,absolute=false;
    bool raw(int which,double x,double &y)
    {
        if(state!=status::ok)return false;
        if(evaluations>=limit){state=status::budget;return false;}
        if(poll&&poll(poll_context,-1)){state=status::cancelled;return false;}
        ++evaluations;int code=sample(context,which,x,0,&y);
        if(code<0){state=code==-2?status::cancelled:status::callback_error;return false;}
        if(code!=1||!std::isfinite(y)){state=status::domain;return false;}return true;
    }
    bool value(double x,double &y)
    {
        if(!raw(curve,x,y))return false;
        if(difference){double second;if(!raw(other,x,second))return false;y-=second;}
        if(absolute)y=std::fabs(y);
        if(!std::isfinite(y)){state=status::domain;return false;}return true;
    }
    bool recoverable() const {return state==status::domain;}
};
inline bool opposite(double a,double b){return (a<0&&b>0)||(a>0&&b<0);}
inline double midpoint(double a,double b){return a/2+b/2;}
inline bool zero(evaluator &e,double a,double b,double fa,double fb,double &x)
{
    double scale=std::fmax(std::fabs(fa),std::fabs(fb));
    double xtolerance=std::fmax((b-a)*1e-11,32*DBL_EPSILON*std::fmax(std::fabs(a),std::fabs(b)));
    for(int n=0;n<160;++n)
    {
        double m=midpoint(a,b),fm;if(!e.value(m,fm))return false;
        if(fm==0||(std::fabs(fm)<=scale*1e-10&&std::fabs(b-a)<=xtolerance)){x=m;return true;}
        if(m==a||m==b)break;
        if(opposite(fa,fm)){b=m;fb=fm;}else{a=m;fa=fm;}
    }
    return false; // A shrinking bracket alone does not establish a root (poles).
}
inline bool optimise(evaluator &e,double a,double b,bool maximum,double &x,double &y,double &error,bool absolute=false)
{
    constexpr double ratio=0.6180339887498948482;
    double xtolerance=std::fmax((b-a)*1e-11,32*DBL_EPSILON*std::fmax(std::fabs(a),std::fabs(b)));
    double c=b-ratio*(b-a),d=a+ratio*(b-a),fc,fd;
    if(!e.value(c,fc)||!e.value(d,fd))return false;
    auto score=[&](double v){return absolute?std::fabs(v):maximum?-v:v;};
    for(int n=0;n<128;++n)
    {
        if(c==d||midpoint(a,b)==a||midpoint(a,b)==b)break;
        if(!absolute&&std::fabs(b-a)<=xtolerance)break;
        if(score(fc)<score(fd)){b=d;d=c;fd=fc;c=b-ratio*(b-a);if(!e.value(c,fc))return false;}
        else{a=c;c=d;fc=fd;d=a+ratio*(b-a);if(!e.value(d,fd))return false;}
    }
    x=score(fc)<score(fd)?c:d;y=score(fc)<score(fd)?fc:fd;error=std::fabs(b-a);
    if(!absolute&&std::fabs(fc-fd)>1e-5*std::fmax(1.0,std::fabs(y))){e.state=status::not_converged;return false;}
    return true;
}
inline void append(result &out,evaluator &e,double x,double separation)
{
    for(int i=0;i<out.count;++i)if(std::fabs(out.points[i].x-x)<=separation)return;
    if(out.count==16){out.truncated=true;return;}
    double y;if(!e.raw(e.curve,x,y))return;int at=out.count++;while(at&&out.points[at-1].x>x){out.points[at]=out.points[at-1];--at;}out.points[at]={x,y};
}
inline void find_roots(evaluator &e,double a,double b,result &out)
{
    double values[257];bool valid[257];
    for(int i=0;i<=256;++i)
    {
        valid[i]=e.value(pico_plot_engine::coordinate(a,b,i,256),values[i]);
        if(!valid[i]){if(!e.recoverable())return;e.state=status::ok;}
    }
    bool all_zero=true;for(int i=0;i<=256;++i)all_zero=all_zero&&valid[i]&&values[i]==0;
    if(all_zero){e.state=status::not_converged;return;}
    double separation=(b-a)*1e-10+32*DBL_EPSILON*std::fmax(std::fabs(a),std::fabs(b));
    for(int i=0;i<=256;++i)
    {
        double x=pico_plot_engine::coordinate(a,b,i,256);
        if(valid[i]&&values[i]==0)append(out,e,x,separation);
        if(e.state!=status::ok||out.truncated)return;
        if(i&&valid[i-1]&&valid[i]&&opposite(values[i-1],values[i]))
        {
            double root;
            if(zero(e,pico_plot_engine::coordinate(a,b,i-1,256),x,values[i-1],values[i],root))append(out,e,root,separation);
            else if(e.recoverable())e.state=status::ok;
            if(e.state!=status::ok||out.truncated)return;
        }
        // Tangent roots require an exactly zero evaluation, not just a tiny minimum.
        if(i>0&&i<256&&valid[i-1]&&valid[i]&&valid[i+1]&&std::fabs(values[i])<std::fabs(values[i-1])&&std::fabs(values[i])<std::fabs(values[i+1])&&values[i]!=0)
        {
            double root,y,error;
            if(optimise(e,pico_plot_engine::coordinate(a,b,i-1,256),pico_plot_engine::coordinate(a,b,i+1,256),false,root,y,error,true)&&y==0)append(out,e,root,separation);
            else if(e.recoverable())e.state=status::ok;
            if(e.state!=status::ok||out.truncated)return;
        }
    }
    if(!out.count)e.state=status::no_result;
}
inline void extremum(evaluator &e,double a,double b,bool maximum,result &out)
{
    int best=0;double value=0;
    for(int i=0;i<=256;++i)
    {
        double y;if(!e.value(pico_plot_engine::coordinate(a,b,i,256),y))return;
        if(!i||(maximum?y>value:y<value)){value=y;best=i;}
    }
    double x=pico_plot_engine::coordinate(a,b,best,256),error=0;
    if(best>0&&best<256)
    {
        double rx,ry;
        if(!optimise(e,pico_plot_engine::coordinate(a,b,best-1,256),pico_plot_engine::coordinate(a,b,best+1,256),maximum,rx,ry,error))return;
        if(maximum?ry>value:ry<value){x=rx;value=ry;}
    }
    out.points[0]={x,value};out.count=1;out.value=value;out.error=error;
}
inline void integrate(evaluator &e,double a,double b,result &out)
{
    struct interval {double a,b,fa,fm,fb,estimate,tolerance;int depth;};
    interval work[26];int used=0;double fa,fm,fb;
    if(!e.value(a,fa)||!e.value(midpoint(a,b),fm)||!e.value(b,fb))return;
    double initial=(b-a)*(fa/6+2*fm/3+fb/6);
    if(!std::isfinite(initial)){e.state=status::not_converged;return;}
    work[used++]={a,b,fa,fm,fb,initial,1e-8*std::fmax(1.0,std::fabs(initial)),0};
    double sum=0,correction=0,error=0;
    while(used)
    {
        interval v=work[--used];double m=midpoint(v.a,v.b),left=midpoint(v.a,m),right=midpoint(m,v.b),fl,fr;
        if(left==v.a||right==v.b){e.state=status::not_converged;return;}
        if(!e.value(left,fl)||!e.value(right,fr))return;
        double l=(m-v.a)*(v.fa/6+2*fl/3+v.fm/6),r=(v.b-m)*(v.fm/6+2*fr/3+v.fb/6),delta=l+r-v.estimate;
        if(!std::isfinite(delta)){e.state=status::not_converged;return;}
        if(v.depth>=4&&std::fabs(delta)<=15*v.tolerance)
        {
            double term=l+r+delta/15-correction,next=sum+term;correction=(next-sum)-term;sum=next;error+=std::fabs(delta)/15;
        }
        else
        {
            if(v.depth>=24||used+2>26){e.state=status::not_converged;return;}
            work[used++]={m,v.b,v.fm,fr,v.fb,r,v.tolerance/2,v.depth+1};
            work[used++]={v.a,m,v.fa,fl,v.fm,l,v.tolerance/2,v.depth+1};
        }
    }
    if(!std::isfinite(sum)){e.state=status::not_converged;return;}
    out.value=sum;out.error=error;
}
inline result run(operation op,pico_plot_sample sample,void *context,int curve,int other,double a,double b,double at,
                  pico_plot_engine::poll_function poll=nullptr,void *poll_context=nullptr,int evaluation_limit=8192)
{
    result out;if(evaluation_limit<1||evaluation_limit>8192||!sample||curve<0||curve>2||other<0||other>2||!std::isfinite(a)||!std::isfinite(b)||!std::isfinite(at)||std::fabs(a)>1e100||std::fabs(b)>1e100||std::fabs(at)>1e100)return out;
    if(op!=operation::derivative&&a==b){if(op==operation::integral||op==operation::area||op==operation::between)out.state=status::ok;return out;}
    bool reversed=a>b;if(reversed){double swap=a;a=b;b=swap;}
    evaluator e{sample,context,curve,other,poll,poll_context};e.limit=evaluation_limit;
    if(op==operation::intersections||op==operation::between){if(curve==other)return out;e.difference=true;}
    if(op==operation::roots||op==operation::intersections)find_roots(e,a,b,out);
    else if(op==operation::minimum||op==operation::maximum)extremum(e,a,b,op==operation::maximum,out);
    else if(op==operation::integral||op==operation::area||op==operation::between){e.absolute=op==operation::area||op==operation::between;integrate(e,a,b,out);if(reversed&&!e.absolute)out.value=-out.value;}
    else
    {
        double h=std::cbrt(DBL_EPSILON)*std::fmax(1.0,std::fabs(at)),centre;
        if(e.value(at,centre))
        {
            bool converged=false;status last=status::not_converged;
            for(int attempt=0;attempt<12&&!converged;++attempt,h/=4)
            {
                double p,m,p2,m2;
                if(!(e.value(at+h,p)&&e.value(at-h,m)&&e.value(at+h/2,p2)&&e.value(at-h/2,m2)))
                {if(!e.recoverable())break;last=e.state;e.state=status::ok;continue;}
                double d=(p-m)/(2*h),d2=(p2-m2)/h,left=(centre-m2)/(h/2),right=(p2-centre)/(h/2);
                out.value=(4*d2-d)/3;out.error=std::fabs(d2-d)/3;
                converged=std::isfinite(out.value)&&out.error<=1e-5*std::fmax(1.0,std::fabs(out.value))&&std::fabs(left-right)<=1e-3*std::fmax(1.0,std::fabs(out.value));
                last=status::not_converged;
            }
            if(!converged&&e.state==status::ok)e.state=last;
        }
    }
    out.state=e.state;out.evaluations=e.evaluations;return out;
}
inline const char *message(status state)
{
    switch(state){case status::ok:return "Complete";case status::no_result:return "No roots resolved in this interval";case status::domain:return "Undefined or non-real sample";case status::not_converged:return "Result did not converge / not isolated";case status::cancelled:return "Interrupted";case status::callback_error:return "Function evaluation failed";case status::budget:return "Evaluation limit reached";default:return "Invalid interval or curve selection";}
}
}
