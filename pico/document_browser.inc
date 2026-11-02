// Native file chooser shared by bounded data/document tools.
namespace {
bool document_file(const char *folder,const char *extension,char *path,size_t capacity)
{
    int &selected=pico_ui::focus(folder).selected;
    for(;;)
    {
        char names[8][256]={};int total=0,offset=selected/8*8;DIR directory;FILINFO info;
        FRESULT result=pico_f_opendir(&directory,folder);if(result!=FR_OK){notice("Files","Cannot read directory");return false;}
        while((result=f_readdir(&directory,&info))==FR_OK&&*info.fname)
        {
            if(!file_visible(info))continue;
            char name[256];snprintf(name,sizeof name,"%s",info.fname);char *ext=strrchr(name,'.');if(!ext)continue;
            if((!strcasecmp(ext,".bak")||!strcasecmp(ext,".rollback"))){bool old_backup=!strcasecmp(ext,".bak");*ext=0;ext=strrchr(name,'.');if(!ext||strcasecmp(ext,extension))continue;char primary[320];snprintf(primary,sizeof primary,"%s/%s",folder,name);if(file_exists(primary))continue;if(old_backup){char pending[340];snprintf(pending,sizeof pending,"%s.rollback",primary);if(file_exists(pending))continue;}}
            else if(strcasecmp(ext,extension))continue;
            if(total>=offset&&total<offset+8)snprintf(names[total-offset],256,"%s",name);++total;
        }
        f_closedir(&directory);if(result!=FR_OK){notice("Files","Directory read failed");return false;}
        if(total&&selected>=total){selected=total-1;continue;}
        screen_page("document_browser.document_file.page1",folder,pico_ui::Actions{{total?"Open":nullptr,nullptr,nullptr,total?"Details":nullptr,"Back"}});
        for(int i=0;i<8&&i+offset<total;++i)file_row_label(i,names[i],i+offset==selected);
        list_page_hint(selected,total);
        if(!total)info_label(8,70,"None saved yet. Save from the tool Files menu.");flush_display();int key=read_key_raw();file_row_key(key,selected,total);
        if(key==27||key==0xb1||key==0x85)return false;
        if(key==0xb5&&total)selected=(selected+total-1)%total;
        else if(key==0xb6&&total)selected=(selected+1)%total;
        else if(key==0xb4&&total)selected=std::max(0,(selected/8-1)*8);
        else if(key==0xb7&&total)selected=std::min(total-1,(selected/8+1)*8);
        else if(key==0x84&&total)screen_text("files.details","File details",names[selected-offset],strlen(names[selected-offset]));
        else if((key==10||key==13||key==0x81)&&total)return snprintf(path,capacity,"%s/%s",folder,names[selected-offset])<int(capacity);
    }
}
bool document_save_name(const char *folder,const char *extension,char *path,size_t capacity)
{
    char name[40]="LAST";if(!prompt("Save name",name,sizeof name,true))return false;
    if(snprintf(path,capacity,"%s/%s%s",folder,name,extension)>=int(capacity))return false;
    return !file_exists(path)||confirm("Replace saved file?");
}
void document_result(object_p result,const char *title)
{
    screen_result("document_browser.document_result.value_result1",result,title);
}
}
