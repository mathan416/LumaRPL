#pragma once
#include "svg_export.h"
#include "storage.h"
struct pico_graph_export
{
    FIL file{};char destination[320]{},temporary[330]{};bool opened=false,failed=false,cancelled=false;
    bool (*poll)()=nullptr;pico_svg::writer svg;
    pico_graph_export():svg{write,this}{}
    static bool write(void *opaque,const char *data,size_t size)
    {
        auto &self=*static_cast<pico_graph_export *>(opaque);if(self.failed||!self.opened)return false;
        if(self.poll&&self.poll()){self.cancelled=self.failed=true;return false;}
        UINT written=0;if(pico_f_write(&self.file,data,size,&written)!=FR_OK||written!=size)self.failed=true;return !self.failed;
    }
    bool open(const char *path,bool (*cancel)()=nullptr)
    {
        if(strlen(path)>=sizeof destination||snprintf(temporary,sizeof temporary,"%s.tmp",path)>=int(sizeof temporary))return false;
        strcpy(destination,path);poll=cancel;pico_io_reset();opened=pico_f_open(&file,temporary,FA_WRITE|FA_CREATE_ALWAYS)==FR_OK;return opened;
    }
    bool put(const char *text){return write(this,text,strlen(text));}
    bool commit()
    {
        if(!opened)return false;if(pico_f_close(&file)!=FR_OK)failed=true;opened=false;
        return !failed&&!pico_io_failed()&&pico_commit_state(temporary,destination);
    }
    ~pico_graph_export(){if(opened)pico_f_close(&file);}
};
