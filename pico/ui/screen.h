// Screen identity and bounded observation hooks; no frame capture in firmware.
#pragma once
namespace pico_ui {
inline const char *active_screen=nullptr,*visible_screen="calculator.stack";
using Observer=void(*)(const char *,const char *);
inline Observer observer=nullptr;
using MenuObserver=void(*)(const char *,const char *const *,int,int);
inline MenuObserver menu_observer=nullptr;
struct ScreenScope {
    const char *previous;
    explicit ScreenScope(const char *id):previous(active_screen){active_screen=id;}
    ~ScreenScope(){active_screen=previous;}
};
inline void seen(const char *title){visible_screen=active_screen?active_screen:"shell.unscoped";if(observer)observer(visible_screen,title);}
}
