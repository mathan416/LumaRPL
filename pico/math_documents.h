#pragma once
#include "form_document.h"
#include "finance_math.h"
struct pico_matrix_document {
 unsigned rows=2,columns=2;bool dirty=false;char cells[6][6][32]{};
 pico_matrix_document(){reset(2,2);dirty=false;}
 void reset(unsigned r,unsigned c){rows=r;columns=c;for(auto &row:cells)for(auto &v:row)strcpy(v,"0");for(unsigned i=0;i<r&&i<c;++i)strcpy(cells[i][i],"1");dirty=true;}
 static constexpr size_t bytes=1168;
 bool valid()const {if(rows<1||rows>6||columns<1||columns>6)return false;for(auto &row:cells)for(auto &v:row)if(!pico_form_document::text(v,32))return false;return true;}
 size_t encode(uint8_t *out)const{if(!valid())return 0;memset(out,0,bytes);memcpy(out,"LUMAMAT",7);out[7]=1;out[8]=rows;out[9]=columns;memcpy(out+12,cells,sizeof cells);uint32_t crc=pico_graph_scene::checksum(out,bytes-4);for(int i=0;i<4;++i)out[bytes-4+i]=crc>>(8*i);return bytes;}
 bool decode(const uint8_t *in,size_t n){if(n!=bytes||memcmp(in,"LUMAMAT",7)||in[7]!=1||in[10]||in[11])return false;uint32_t crc=0;for(int i=0;i<4;++i)crc|=uint32_t(in[bytes-4+i])<<(8*i);if(crc!=pico_graph_scene::checksum(in,bytes-4))return false;pico_matrix_document next;next.rows=in[8];next.columns=in[9];memcpy(next.cells,in+12,sizeof cells);if(!next.valid())return false;next.dirty=false;*this=next;return true;}
};
struct pico_finance_document {
 double principal=10000,annual=5,periods=60,frequency=12,discount=5,flows[17]={-1000,400,400,400};unsigned count=4;bool beginning=false,dirty=false;
 double repayment=188.7123364401093,deposit=100,irr_low=0,irr_high=100;
 static constexpr size_t legacy_bytes=192,bytes=224;
 bool valid()const{if(!pico_finance::inputs(principal,annual,periods,frequency)||!std::isfinite(repayment)||repayment<0||!std::isfinite(deposit)||deposit<0||!std::isfinite(irr_low)||!std::isfinite(irr_high)||irr_low<=-100||irr_high>10000||irr_low>=irr_high||count<1||count>17||!std::isfinite(discount)||discount<=-100||discount>10000)return false;for(double x:flows)if(!std::isfinite(x))return false;return true;}
 size_t encode(uint8_t *out)const{if(!valid())return 0;memset(out,0,bytes);memcpy(out,"LUMAFIN",7);out[7]=2;out[8]=count;out[9]=beginning;double values[26]={principal,annual,periods,frequency,discount};memcpy(values+5,flows,sizeof flows);values[22]=repayment;values[23]=deposit;values[24]=irr_low;values[25]=irr_high;for(unsigned i=0;i<26;++i){uint64_t bits;memcpy(&bits,values+i,8);for(int b=0;b<8;++b)out[12+i*8+b]=bits>>(b*8);}uint32_t crc=pico_graph_scene::checksum(out,bytes-4);for(int i=0;i<4;++i)out[bytes-4+i]=crc>>(i*8);return bytes;}
 bool decode(const uint8_t *in,size_t n){bool legacy=n==legacy_bytes&&in[7]==1;if((!legacy&&(n!=bytes||in[7]!=2))||memcmp(in,"LUMAFIN",7)||in[9]>1||in[10]||in[11])return false;uint32_t crc=0;for(int i=0;i<4;++i)crc|=uint32_t(in[n-4+i])<<(i*8);if(crc!=pico_graph_scene::checksum(in,n-4))return false;double values[26]{};for(unsigned i=0;i<(legacy?22u:26u);++i){uint64_t bits=0;for(int b=0;b<8;++b)bits|=uint64_t(in[12+i*8+b])<<(8*b);memcpy(values+i,&bits,8);}pico_finance_document next;next.principal=values[0];next.annual=values[1];next.periods=values[2];next.frequency=values[3];next.discount=values[4];memcpy(next.flows,values+5,sizeof flows);if(!legacy){next.repayment=values[22];next.deposit=values[23];next.irr_low=values[24];next.irr_high=values[25];}next.count=in[8];next.beginning=in[9];if(!next.valid())return false;next.dirty=false;*this=next;return true;}
};
