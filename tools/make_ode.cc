#include "ode.h"
#include <cstdio>
int main(int argc,char **argv)
{
    if(argc!=3)return 2;pico_ode_scene scene;
    if(!strcmp(argv[2],"logistic")){strcpy(scene.formula,"Y*(1-Y)");scene.y0=.1;double w[]={-2,8,-.2,1.2};memcpy(scene.window,w,sizeof w);}
    else if(strcmp(argv[2],"exponential"))return 2;
    uint8_t data[pico_ode_scene::bytes];size_t n=scene.encode(data,sizeof data);FILE *file=fopen(argv[1],"wb");if(!file)return 1;bool ok=n&&fwrite(data,1,n,file)==n;if(fclose(file))ok=false;return ok?0:1;
}
