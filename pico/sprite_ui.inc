// Colour sprite document and CHIP-8 mask editor; bounded, no extra framebuffer.
namespace {
pico_sprite sprite_document;
bool sprite_dirty=false,sprite_colour=true;
uint8_t sprite_paint=0x7c;bool sprite_erase=false;
bool document_file(const char *,const char *,char *,size_t);
bool document_save_name(const char *,const char *,char *,size_t);
bool save_sprite_document()
{
    if(!sys_disk_ok()){notice("Save unavailable","Insert the SD card; drawing remains in memory");return false;}
    char path[320];if(!document_save_name("sprites",".sprite",path,sizeof path))return false;
    bool ok=pico_sprite_save(path,sprite_document);if(ok)sprite_dirty=false;else notice("Sprite save failed","Previous file retained; drawing remains in memory");return ok;
}
bool replace_sprite()
{
    if(!sprite_dirty)return true;int action=unsaved_source("Unsaved sprite");return action==2||(action==1&&save_sprite_document());
}
void sprite_palette()
{
    unsigned selected=sprite_paint;
    for(;;){screen_page("sprite.palette","Sprite colour",pico_ui::Actions{{"Paint",nullptr,"Erase",nullptr,"Back"}});
        for(unsigned i=0;i<256;++i){uint32_t c=pico_sprite::rgb(i);Screen.fill(16+(i%16)*18,36+(i/16)*14,32+(i%16)*18,48+(i/16)*14,pattern(c>>16,(c>>8)&255,c&255));}
        int x=16+(selected%16)*18,y=36+(selected/16)*14;Screen.line(x,y,x+16,y,1,pattern::white);Screen.line(x,y+12,x+16,y+12,1,pattern::black);
        char status[80];snprintf(status,sizeof status,"Colour %u | Arrows choose | Enter paints",selected);info_label(8,269,status);flush_display();int k=read_key_raw();
        if(k==0x85||k==27||k==0xb1)return;
        if(k==0x83){sprite_erase=true;return;}if(k==0x81||k==13||k==10){sprite_paint=selected;sprite_erase=false;return;}
        if(k==0xb4)selected=(selected+255)%256;if(k==0xb7)selected=(selected+1)%256;if(k==0xb5)selected=(selected+240)%256;if(k==0xb6)selected=(selected+16)%256;
    }
}
void sprite_export()
{
    if(sprite_colour&&!confirm("Export opacity mask? Opaque pixels become set bits; transparent pixels become clear bits. Colours are not included."))return;
    if(!sys_disk_ok()){notice("Export unavailable","Insert an SD card");return;}
    char path[320];if(!document_save_name("chip8",".asm",path,sizeof path))return;
    char source[1024]="HIGH\nCLS\nLD V0, 48\nLD V1, 24\nLD I, sprite\nDRW V0, V1, 0\nwait:\nJP wait\nsprite:\n";
    for(int y=0;y<16;++y){unsigned row=0;for(int x=0;x<16;++x)if(sprite_document.opaque(y*16+x))row|=0x8000>>x;char line[32];snprintf(line,sizeof line,"DB 0x%02X, 0x%02X\n",row>>8,row&255);strcat(source,line);}
    bool ok=save_source(path,source);notice(ok?"Mask exported":"Export failed",ok?"Open CHIP-8 / assembly to build. Save the .sprite document to retain colours.":"Previous file retained");
}
void sprite_editor()
{
    int x=0,y=0;pico_sprite original=sprite_document;bool original_colour=sprite_colour;
    for(;;){char title[64];snprintf(title,sizeof title,"%s sprite%s",sprite_colour?"Colour":"CHIP-8",sprite_dirty?" *":"");
        screen_page("desktop.sprite_editor.page1",title,pico_ui::Actions{{"Save","Colour",nullptr,"Tools","Back"}});
        for(unsigned i=0;i<256;++i){uint32_t rgb=pico_sprite::rgb(sprite_colour?sprite_document.pixels[i]:sprite_paint);pattern c=sprite_document.opaque(i)?pattern(rgb>>16,(rgb>>8)&255,rgb&255):pattern((i+i/16)%2?45:65,(i+i/16)%2?45:65,(i+i/16)%2?45:65);Screen.fill(64+(i%16)*12,40+(i/16)*12,74+(i%16)*12,50+(i/16)*12,c);}
        Screen.line(64+x*12,40+y*12,74+x*12,40+y*12,1,pattern::white);Screen.line(64+x*12,50+y*12,74+x*12,50+y*12,1,pattern::black);
        info_label(8,249,sprite_erase?"Erase | Space/Enter clears":sprite_colour?"Paint | Space/Enter applies colour":"Mask | Space/Enter toggles");info_label(8,269,"Arrows move | F2 palette | F4 tools");flush_display();int k=read_key_raw();
        if(k==0x85||k==27||k==0xb1){if(!sprite_dirty)break;int action=unsaved_source("Unsaved sprite");if(action==2){sprite_document=original;sprite_colour=original_colour;sprite_dirty=false;break;}if(action==1&&save_sprite_document())break;continue;}
        if(k==0xb4)x=(x+15)%16;else if(k==0xb7)x=(x+1)%16;else if(k==0xb5)y=(y+15)%16;else if(k==0xb6)y=(y+1)%16;
        else if(k==' '||k==10||k==13){sprite_document.paint(y*16+x,sprite_paint,sprite_erase,!sprite_colour);sprite_dirty=true;}
        else if(k==8||k==127){sprite_document.opaque(y*16+x,false);sprite_dirty=true;}
        else if(k==0x81){if(save_sprite_document()){original=sprite_document;original_colour=sprite_colour;}}else if(k==0x82)sprite_palette();
        else if(k==0x84){static const char *const items[]={"Open sprite","New colour sprite","CHIP-8 mode","Colour mode","Clear drawing","Export CHIP-8 mask","Help"};int a=screen_menu("sprite.tools","Sprite tools",items,7);
            if(a==0){if(!replace_sprite())continue;if(!sprite_dirty){original=sprite_document;original_colour=sprite_colour;}char path[320];if(!document_file("sprites",".sprite",path,sizeof path))continue;pico_sprite next;bool backup=false;if(!pico_sprite_load(path,next,backup)){notice("Cannot open sprite","Invalid or unavailable file; current drawing kept");continue;}sprite_document=next;sprite_colour=true;sprite_dirty=false;original=sprite_document;original_colour=true;if(backup)notice("Sprite recovered","Loaded the previous valid backup");}
            if(a==1&&replace_sprite()){sprite_document=pico_sprite{};sprite_colour=true;sprite_dirty=false;original=sprite_document;original_colour=true;}
            if(a==2&&(!sprite_colour||sprite_document.empty()||confirm("Switch to CHIP-8 mask mode? Pixel colours will be replaced by the preview colour."))){for(unsigned i=0;i<256;++i)sprite_document.pixels[i]=sprite_paint;sprite_colour=false;sprite_dirty=true;}
            if(a==3)sprite_colour=true;
            if(a==4&&confirm("Clear the drawing?")){sprite_document=pico_sprite{};sprite_dirty=true;}
            if(a==5)sprite_export();
            if(a==6){const char *help=pico_ui_help::tool_text("sprite.help");screen_text("sprite.help","Sprite editor",help,strlen(help));}
        }
    }key_pop_all();
}
}
