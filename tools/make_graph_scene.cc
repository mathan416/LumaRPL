#include "graph_scene.h"
#include <cstdio>
int main(int argc,char **argv)
{
    if(argc!=2&&argc!=3)return 2;pico_graph_scene scene;scene.multiple=1;scene.angle=1;
    scene.window[0]=-3;scene.window[1]=3;scene.window[2]=-2;scene.window[3]=4;
    if(argc==3){if(strcmp(argv[2],"analysis"))return 2;snprintf(scene.curve[0],160,"X^2-4");scene.window[2]=-5;scene.window[3]=6;}
    uint8_t data[1024];size_t size=scene.encode(data,sizeof data);if(!size)return 1;
    FILE *f=fopen(argv[1],"wb");if(!f)return 1;bool ok=fwrite(data,1,size,f)==size;if(fclose(f))ok=false;return ok?0:1;
}
