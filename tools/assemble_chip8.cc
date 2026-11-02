#include "chip8_assembler.h"
#include <cstdio>
#include <cstdlib>
int main(int argc,char **argv)
{
    if(argc!=3){fprintf(stderr,"Usage: assemble-chip8 source.asm output.ch8\n");return 2;}
    FILE *f=fopen(argv[1],"rb");if(!f){perror(argv[1]);return 1;}
    char source[16384];size_t n=fread(source,1,sizeof source,f);bool ok=!ferror(f)&&n<sizeof source;fclose(f);
    if(!ok){fprintf(stderr,"Source must be under 16 KiB\n");return 1;}source[n]=0;
    pico_chip8::assembler compiler;unsigned char rom[3584];size_t size;
    if(!compiler.assemble(source,rom,sizeof rom,size)){fprintf(stderr,"Line %u: %s\n",compiler.line,compiler.error);return 1;}
    f=fopen(argv[2],"wb");if(!f){perror(argv[2]);return 1;}ok=fwrite(rom,1,size,f)==size;if(fclose(f))ok=false;
    if(!ok){fprintf(stderr,"ROM write failed\n");return 1;}printf("%zu bytes assembled\n",size);
}
