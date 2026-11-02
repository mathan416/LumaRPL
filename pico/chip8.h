#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
namespace pico_chip8 {
enum profile { vip,chip48,schip,schip_legacy };
struct frame_clock {
    uint64_t next;
    unsigned due(uint64_t now)
    {
        if(now<next)return 0;
        uint64_t frames=(now-next)/16667+1;
        if(frames>6){next=now+16667;return 6;}
        next+=frames*16667;return unsigned(frames);
    }
};
struct machine {
    uint8_t memory[4096]{},screen[1024]{},v[16]{},flags[8]{};
    uint16_t stack[16]{},pc=0x200,last_pc=0x200,index=0,keys=0,previous_keys=0;
    uint8_t sp=0,delay=0,sound=0;
    profile mode=schip;
    bool high=false,dirty=true,exited=false,draw_wait=false;
    int waiting=-1,latched=-1;
    uint32_t random_state=0x12345678;
    const char *error=nullptr;
    static constexpr uint8_t font[80]={
      0xf0,0x90,0x90,0x90,0xf0,0x20,0x60,0x20,0x20,0x70,
      0xf0,0x10,0xf0,0x80,0xf0,0xf0,0x10,0xf0,0x10,0xf0,
      0x90,0x90,0xf0,0x10,0x10,0xf0,0x80,0xf0,0x10,0xf0,
      0xf0,0x80,0xf0,0x90,0xf0,0xf0,0x10,0x20,0x40,0x40,
      0xf0,0x90,0xf0,0x90,0xf0,0xf0,0x90,0xf0,0x10,0xf0,
      0xf0,0x90,0xf0,0x90,0x90,0xe0,0x90,0xe0,0x90,0xe0,
      0xf0,0x80,0x80,0x80,0xf0,0xe0,0x90,0x90,0x90,0xe0,
      0xf0,0x80,0xf0,0x80,0xf0,0xf0,0x80,0xf0,0x80,0x80};
    bool load(const uint8_t *rom,size_t size,profile p)
    {
        *this=machine{};mode=p;
        if(!size || size>3584) { error="ROM must be 1..3584 bytes";return false; }
        memcpy(memory+0x50,font,sizeof font);memcpy(memory+0x200,rom,size);
        for(int digit=0;digit<16;++digit) for(int row=0;row<5;++row)
        {
            uint8_t value=font[digit*5+row],expanded=0;
            for(int bit=0;bit<4;++bit) if(value&(0x80>>bit)) expanded|=0xc0>>(bit*2);
            memory[0xa0+digit*10+row*2]=expanded;memory[0xa0+digit*10+row*2+1]=expanded;
        }
        return true;
    }
    bool pixel(int x,int y) const { return screen[y*16+x/8]&(0x80>>(x%8)); }
    bool toggle(int x,int y)
    { uint8_t &b=screen[y*16+x/8];uint8_t bit=0x80>>(x%8);bool old=b&bit;b^=bit;return old; }
    void set_pixel(int x,int y,bool value)
    { uint8_t &b=screen[y*16+x/8];uint8_t bit=0x80>>(x%8);if(value)b|=bit;else b&=~bit; }
    void scroll(int dx,int dy)
    {
        int y=dy>0?63:0,end=dy>0?-1:64,step=dy>0?-1:1;
        for(;y!=end;y+=step)
        {
            int x=dx>0?127:0,xe=dx>0?-1:128,xs=dx>0?-1:1;
            for(;x!=xe;x+=xs)
            { int sx=x-dx,sy=y-dy;set_pixel(x,y,sx>=0&&sx<128&&sy>=0&&sy<64&&pixel(sx,sy)); }
        }
        dirty=true;
    }
    void set_keys(uint16_t value)
    {
        previous_keys=keys;keys=value;
        if(waiting>=0)
        {
            if(latched<0)
            { uint16_t pressed=keys&~previous_keys;for(int k=0;k<16;++k)if(pressed&(1u<<k)){latched=k;break;} }
            else if(!(keys&(1u<<latched))) { v[waiting]=latched;waiting=latched=-1;pc+=2; }
        }
    }
    void tick() { if(delay)--delay;if(sound)--sound;draw_wait=false; }
    bool range(unsigned at,unsigned count)
    { if(at>4096 || count>4096-at){error="Memory address out of range";return false;}return true; }
    bool step()
    {
        if(error||exited||waiting>=0||draw_wait) return false;
        last_pc=pc;if(!range(pc,2))return false;
        unsigned op=(memory[pc]<<8)|memory[pc+1],x=(op>>8)&15,y=(op>>4)&15,n=op&15,kk=op&255,nnn=op&4095;
        pc+=2;unsigned a=v[x],b=v[y];
        switch(op>>12)
        {
        case 0:
            if(op==0xe0){memset(screen,0,sizeof screen);dirty=true;}
            else if(op==0xee){if(!sp){error="Return stack underflow";return false;}pc=stack[--sp];}
            else if(mode>=schip && op==0xfd)exited=true;
            else if(mode>=schip && (op==0xfe||op==0xff)){high=op==0xff;memset(screen,0,sizeof screen);dirty=true;}
            else if(mode>=schip && (op&0xfff0)==0xc0)scroll(0,mode==schip&&!high?n*2:n);
            else if(mode>=schip && op==0xfb)scroll(mode==schip&&!high?8:4,0);
            else if(mode>=schip && op==0xfc)scroll(mode==schip&&!high?-8:-4,0);
            else {error="Unsupported system instruction";return false;}
            break;
        case 1:pc=nnn;break;
        case 2:if(sp==16){error="Return stack overflow";return false;}stack[sp++]=pc;pc=nnn;break;
        case 3:if(a==kk)pc+=2;break;
        case 4:if(a!=kk)pc+=2;break;
        case 5:if(n){error="Invalid comparison opcode";return false;}if(a==b)pc+=2;break;
        case 6:v[x]=kk;break;
        case 7:v[x]=a+kk;break;
        case 8:
            switch(n){
            case 0:v[x]=b;break;
            case 1:v[x]=a|b;if(mode==vip)v[15]=0;break;
            case 2:v[x]=a&b;if(mode==vip)v[15]=0;break;
            case 3:v[x]=a^b;if(mode==vip)v[15]=0;break;
            case 4:v[x]=a+b;v[15]=(a+b)>255;break;
            case 5:v[x]=a-b;v[15]=a>=b;break;
            case 6:if(mode==vip)a=b;v[x]=a>>1;v[15]=a&1;break;
            case 7:v[x]=b-a;v[15]=b>=a;break;
            case 0xe:if(mode==vip)a=b;v[x]=a<<1;v[15]=(a>>7)&1;break;
            default:error="Unknown arithmetic opcode";return false;}
            break;
        case 9:if(n){error="Invalid comparison opcode";return false;}if(a!=b)pc+=2;break;
        case 0xa:index=nnn;break;
        case 0xb:pc=nnn+v[mode==vip?0:x];break;
        case 0xc:random_state^=random_state<<13;random_state^=random_state>>17;random_state^=random_state<<5;v[x]=random_state&kk;break;
        case 0xd:{
            int width=high?128:64,height=high?64:32,scale=high?1:2;
            int rows=n?n:(mode>=schip?16:0),cols=n?8:16,bytes=rows*(cols/8);
            if(!rows){error="16x16 sprites need SCHIP";return false;}
            if(!range(index,bytes))return false;
            int ox=a%width,oy=b%height;v[15]=0;
            for(int r=0;r<rows && oy+r<height;++r)for(int c=0;c<cols && ox+c<width;++c)
                if(memory[index+r*(cols/8)+c/8]&(0x80>>(c%8)))
                    for(int yy=0;yy<scale;++yy)for(int xx=0;xx<scale;++xx)
                        if(toggle((ox+c)*scale+xx,(oy+r)*scale+yy))v[15]=1;
            dirty=true;if(mode==vip || (mode==schip_legacy&&!high))draw_wait=true;break;}
        case 0xe:
            if(kk==0x9e){if(a<16 && (keys&(1u<<a)))pc+=2;}
            else if(kk==0xa1){if(a>=16 || !(keys&(1u<<a)))pc+=2;}
            else {error="Unknown key opcode";return false;}break;
        case 0xf:
            switch(kk){
            case 7:v[x]=delay;break;
            case 0xa:waiting=x;latched=-1;pc-=2;break;
            case 0x15:delay=a;break;case 0x18:sound=a;break;
            case 0x1e:index=(index+a)&4095;break;
            case 0x29:index=0x50+(a&15)*5;break;
            case 0x30:if(mode<schip){error="Large font needs SCHIP";return false;}index=0xa0+(a&15)*10;break;
            case 0x33:if(!range(index,3))return false;memory[index]=a/100;memory[index+1]=(a/10)%10;memory[index+2]=a%10;break;
            case 0x55:case 0x65:
                if(!range(index,x+1))return false;
                for(unsigned j=0;j<=x;++j){if(kk==0x55)memory[index+j]=v[j];else v[j]=memory[index+j];}
                if(mode==vip)index=(index+x+1)&4095;else if(mode==chip48)index=(index+x)&4095;break;
            case 0x75:case 0x85:
                if(mode<schip||x>7){error="Invalid SCHIP flag register";return false;}
                for(unsigned j=0;j<=x;++j){if(kk==0x75)flags[j]=v[j];else v[j]=flags[j];}break;
            default:error="Unknown register opcode";return false;}
            break;
        default:error="Unknown opcode";return false;
        }
        return true;
    }
};
inline int key_index(int c)
{
    if(c>='A'&&c<='Z')c+=32;
    const char *keys="1234qwerasdfzxcv";const uint8_t map[]={1,2,3,12,4,5,6,13,7,8,9,14,10,0,11,15};
    for(int i=0;i<16;++i)if(c==keys[i])return map[i];return -1;
}
}
