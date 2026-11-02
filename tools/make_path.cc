#include "path_scene.h"
#include <cstdio>
#include <cstring>
int main(int argc,char **argv)
{
    if(argc!=3)return 2;pico_path_scene scene;
    if(!strcmp(argv[2],"ellipse"))strcpy(scene.x,"2*COS(T)");
    else if(!strcmp(argv[2],"rose"))scene.polar=1;
    else if(!strcmp(argv[2],"spiral")){scene.polar=1;strcpy(scene.radius,"T/(2*PI)");scene.last=12.566370614359172;}
    else if(strcmp(argv[2],"circle"))return 2;
    uint8_t data[pico_path_scene::bytes];size_t n=scene.encode(data,sizeof data);FILE *file=fopen(argv[1],"wb");if(!file)return 1;
    bool ok=n&&fwrite(data,1,n,file)==n;if(fclose(file))ok=false;return ok?0:1;
}
