#pragma once
#include "math_documents.h"
struct pico_complex_document {
 char input[4][32]={"3","4","1","-2"};uint8_t polar[2]={0,0},radians=0;bool dirty=false;
 static constexpr size_t bytes=144;
 bool valid()const{if(polar[0]>1||polar[1]>1||radians>1)return false;for(auto &s:input)if(!pico_form_document::text(s,32))return false;return true;}
 size_t encode(uint8_t *out)const{if(!valid())return 0;memset(out,0,bytes);memcpy(out,"LUMACPX",7);out[7]=1;out[8]=polar[0];out[9]=polar[1];out[10]=radians;memcpy(out+12,input,128);uint32_t h=pico_graph_scene::checksum(out,140);for(int i=0;i<4;++i)out[140+i]=h>>(8*i);return bytes;}
 bool decode(const uint8_t *in,size_t n){if(n!=bytes||memcmp(in,"LUMACPX",7)||in[7]!=1||in[11])return false;uint32_t h=0;for(int i=0;i<4;++i)h|=uint32_t(in[140+i])<<(8*i);if(h!=pico_graph_scene::checksum(in,140))return false;pico_complex_document next;memcpy(next.input,in+12,128);next.polar[0]=in[8];next.polar[1]=in[9];next.radians=in[10];if(!next.valid())return false;*this=next;return true;}
};
struct pico_geometry_document {
 double points[6][2]={{-4,-2},{4,2},{-3,3},{3,-3},{0,0},{2,0}};double window[4]={-6,6,-6,6};uint8_t flags=7,selected=0;bool dirty=false;
 static constexpr size_t bytes=144;
 bool valid()const{if(flags>7||selected>=6||!pico_graph_scene::bounds(window))return false;for(auto &p:points)for(double v:p)if(!std::isfinite(v)||std::fabs(v)>1e50)return false;return true;}
 size_t encode(uint8_t *out)const{if(!valid())return 0;memset(out,0,bytes);memcpy(out,"LUMAGEO",7);out[7]=1;out[8]=flags;out[9]=selected;double values[16];memcpy(values,points,96);memcpy(values+12,window,32);for(unsigned i=0;i<16;++i){uint64_t b;memcpy(&b,values+i,8);for(unsigned j=0;j<8;++j)out[12+i*8+j]=b>>(8*j);}uint32_t h=pico_graph_scene::checksum(out,140);for(int i=0;i<4;++i)out[140+i]=h>>(8*i);return bytes;}
 bool decode(const uint8_t *in,size_t n){if(n!=bytes||memcmp(in,"LUMAGEO",7)||in[7]!=1||in[10]||in[11])return false;uint32_t h=0;for(int i=0;i<4;++i)h|=uint32_t(in[140+i])<<(8*i);if(h!=pico_graph_scene::checksum(in,140))return false;double values[16];for(unsigned i=0;i<16;++i){uint64_t b=0;for(unsigned j=0;j<8;++j)b|=uint64_t(in[12+i*8+j])<<(8*j);memcpy(values+i,&b,8);}pico_geometry_document next;memcpy(next.points,values,96);memcpy(next.window,values+12,32);next.flags=in[8];next.selected=in[9];if(!next.valid())return false;*this=next;return true;}
};
