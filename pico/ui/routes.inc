// The same registered views are called by firmware and the host scenarios.
int screen_menu(const char *id,const char *title,const char *const *items,int count,const char *status,const char *focus_key)
{pico_ui::ScreenScope scope(id);return choose_impl(title,items,count,pico_ui::focus(focus_key?focus_key:id),status);}
int screen_text(const char *id,const char *title,const char *text,size_t length,pico_ui::TextRole role,pico_ui::Actions actions)
{pico_ui::ScreenScope scope(id);return view_text(title,text,length,role,actions);}
bool screen_result(const char *id,object_p value,const char *title,const char *text,bool secondary,const char *secondary_name)
{pico_ui::ScreenScope scope(id);return value_result(value,title,text,secondary,secondary_name);}
void screen_page(const char *id,const char *title,pico_ui::Actions actions)
{pico_ui::ScreenScope scope(id);page(title,actions);}
