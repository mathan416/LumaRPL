// Fixed-size RGB332 sprite format. GPL-3.0.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
struct pico_sprite {
    static constexpr size_t bytes=304;
    uint8_t pixels[256]{},mask[32]{};
    bool opaque(unsigned i)const{return i<256&&(mask[i/8]&(1u<<(i%8)));}
    void opaque(unsigned i,bool on){if(i<256){if(on)mask[i/8]|=1u<<(i%8);else mask[i/8]&=~(1u<<(i%8));}}
    void paint(unsigned i,uint8_t colour,bool erase,bool mono=false){if(i>=256)return;if(erase)opaque(i,false);else if(mono)opaque(i,!opaque(i));else{opaque(i,true);pixels[i]=colour;}}
    bool empty()const{for(auto b:mask)if(b)return false;return true;}
    static uint32_t crc(const uint8_t *p,size_t n){uint32_t c=~0u;while(n--){c^=*p++;for(int i=0;i<8;++i)c=(c>>1)^((0u-(c&1))&0xedb88320u);}return ~c;}
    void encode(uint8_t (&out)[bytes])const{memset(out,0,bytes);memcpy(out,"LUMASPR",7);out[8]=1;out[9]=16;out[10]=16;memcpy(out+12,pixels,256);memcpy(out+268,mask,32);uint32_t c=crc(out,300);for(int i=0;i<4;++i)out[300+i]=c>>(i*8);}
    bool decode(const uint8_t *in,size_t n){if(n!=bytes||memcmp(in,"LUMASPR",7)||in[7]||in[8]!=1||in[9]!=16||in[10]!=16||in[11])return false;uint32_t c=0;for(int i=0;i<4;++i)c|=uint32_t(in[300+i])<<(i*8);if(c!=crc(in,300))return false;memcpy(pixels,in+12,256);memcpy(mask,in+268,32);return true;}
    static uint32_t rgb(uint8_t c){unsigned r=c>>5,g=(c>>2)&7,b=c&3;return ((r*255/7)<<16)|((g*255/7)<<8)|(b*255/3);}
    static bool path(const char *name,char *out,size_t capacity){size_t n=strlen(name);if(!n||n>80||name[0]=='/'||strstr(name,"..")||strchr(name,'/')||strchr(name,'\\')||strchr(name,':')||n<8||strcmp(name+n-7,".sprite"))return false;if(n+9>=capacity)return false;memcpy(out,"sprites/",8);memcpy(out+8,name,n+1);return true;}
};
