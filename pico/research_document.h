#pragma once
#include "research_numeric.h"
#include "form_document.h"
#include <memory>
struct pico_research_document {
 bool dirty=false;
 char name[49]="Untitled study",notes[256]{};
 char formula[4][160]={"X^2+Y^2=25","(X-1)^2+(Y+2)^2","EXP(X)","A*B"};
 char units[8][24]={"s","","m","m","","m^2","s",""};
 double window[4]={-8,8,-8,8},levels[3]={0,9,25},bounds[4]={-5,5,-5,5},point[2]={0,0};
 double at=0,first=0,last=1,fs=128,duration=5,frequency_first=.1,frequency_last=100;
 double sigma_first=1,sigma_last=1,confidence=.95,distribution_a=5,distribution_b=1,distribution_x=0,percentile=.95;
 double measurement[3]={2,3,0},sigma[3]={.1,.2,0},correlation[3]{};
 pico_research::matrix matrix;
 pico_research::transfer transfer;
 pico_data_table data[2];
 uint32_t contour_count=1,taylor_degree=4,distribution=2,draws=1024,seed=12345,fit_degree=1,binomial_k=3,binomial_n=10;
 double binomial_p=.5;
 pico_research_document(){matrix.a[0][0]=matrix.a[1][1]=3;matrix.a[0][1]=matrix.a[1][0]=1;data[0].rows=32;for(unsigned i=0;i<32;++i){data[0].points[i][0]=double(i)/fs;data[0].points[i][1]=sin(6.283185307179586*16*i/fs);}data[1].rows=3;for(unsigned i=0;i<3;++i){data[1].points[i][0]=i/fs;data[1].points[i][1]=1./3;}}
 static constexpr size_t bytes=8192;
 template<class IO>void fields(IO &io){io.text(name,sizeof name);io.text(notes,sizeof notes);io.text(&formula[0][0],sizeof formula);io.text(&units[0][0],sizeof units);
 for(auto *p:{&at,&first,&last,&fs,&duration,&frequency_first,&frequency_last,&sigma_first,&sigma_last,&confidence,&distribution_a,&distribution_b,&distribution_x,&percentile,&binomial_p})io.real(*p);
 for(auto &v:window)io.real(v);for(auto &v:levels)io.real(v);for(auto &v:bounds)io.real(v);for(auto &v:point)io.real(v);for(auto &v:measurement)io.real(v);for(auto &v:sigma)io.real(v);for(auto &v:correlation)io.real(v);
 for(auto *p:{&contour_count,&taylor_degree,&distribution,&draws,&seed,&fit_degree,&binomial_k,&binomial_n})io.word(*p);
 io.word(matrix.n);for(auto &row:matrix.a)for(auto &v:row)io.real(v);io.word(transfer.numerator_order);io.word(transfer.denominator_order);for(auto &v:transfer.numerator)io.real(v);for(auto &v:transfer.denominator)io.real(v);
 for(auto &d:data){io.word(d.rows);io.word(d.model);for(auto &row:d.points)for(auto &v:row)io.real(v);}}
 bool valid()const{if(!pico_form_document::text(name,sizeof name)||!pico_form_document::text(notes,sizeof notes,true))return false;for(auto &s:formula)if(!pico_graph_scene::formula(s,false))return false;for(auto &s:units)if(!pico_form_document::text(s,sizeof s,true))return false;
 if(!pico_plot_engine::bounds(window)||!pico_plot_engine::bounds(bounds)||!pico_research::valid(matrix)||!transfer.valid()||!data[0].valid()||!data[1].valid())return false;
 for(double v:{at,first,last,fs,duration,frequency_first,frequency_last,sigma_first,sigma_last,confidence,distribution_a,distribution_b,distribution_x,percentile,binomial_p})if(!std::isfinite(v)||fabs(v)>1e100)return false;
 for(auto &v:levels)if(!std::isfinite(v)||fabs(v)>1e100)return false;for(auto &v:point)if(!std::isfinite(v)||fabs(v)>1e100)return false;for(auto &v:measurement)if(!std::isfinite(v)||fabs(v)>1e100)return false;for(auto &v:sigma)if(!std::isfinite(v)||v<0||v>1e100)return false;
 for(auto &d:data)for(auto &row:d.points)for(double v:row)if(!std::isfinite(v)||fabs(v)>1e100)return false;for(auto &row:matrix.a)for(double v:row)if(!std::isfinite(v)||fabs(v)>1e100)return false;
 double l[3][3];return pico_research::cholesky3(correlation,l)&&contour_count>=1&&contour_count<=3&&taylor_degree<=4&&distribution<=3&&draws>=128&&draws<=2048&&seed&&fit_degree>=1&&fit_degree<=2&&binomial_n<=200&&binomial_k<=binomial_n&&binomial_p>=0&&binomial_p<=1&&fs>0&&duration>0&&frequency_first>0&&frequency_first<frequency_last&&sigma_first>0&&sigma_last>0&&confidence>0&&confidence<1&&percentile>0&&percentile<1;}
 struct writer {uint8_t *out;size_t pos=8;void text(const char *s,size_t n){memcpy(out+pos,s,n);pos+=n;}void word(uint32_t v){for(unsigned i=0;i<4;++i)out[pos++]=v>>(8*i);}void real(double v){uint64_t b;memcpy(&b,&v,8);for(unsigned i=0;i<8;++i)out[pos++]=b>>(8*i);}};
 struct reader {const uint8_t *in;size_t pos=8;void text(char *s,size_t n){memcpy(s,in+pos,n);pos+=n;}template<class T>void word(T &v){static_assert(sizeof(T)==4,"32-bit wire integers");v=0;for(unsigned i=0;i<4;++i)v|=uint32_t(in[pos++])<<(8*i);}void real(double &v){uint64_t b=0;for(unsigned i=0;i<8;++i)b|=uint64_t(in[pos++])<<(8*i);memcpy(&v,&b,8);}};
 size_t encode(uint8_t *out)const{if(!valid())return 0;memset(out,0,bytes);memcpy(out,"LMSTUDY1",8);writer w{out};const_cast<pico_research_document *>(this)->fields(w);if(w.pos>bytes-4)return 0;uint32_t crc=pico_math_lab::checksum(out,bytes-4);for(unsigned i=0;i<4;++i)out[bytes-4+i]=crc>>(8*i);return bytes;}
 bool decode(const uint8_t *in,size_t n){if(n!=bytes||memcmp(in,"LMSTUDY1",8))return false;uint32_t crc=0;for(unsigned i=0;i<4;++i)crc|=uint32_t(in[bytes-4+i])<<(8*i);if(crc!=pico_math_lab::checksum(in,bytes-4))return false;std::unique_ptr<pico_research_document> next(new(std::nothrow) pico_research_document);if(!next)return false;reader r{in};next->fields(r);if(r.pos>bytes-4||!next->valid())return false;for(size_t i=r.pos;i<bytes-4;++i)if(in[i])return false;*this=*next;return true;}
};
static_assert(sizeof(pico_research_document)<7000,"Keep study inputs bounded in the existing heap");
