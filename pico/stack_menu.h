#pragma once
// Keep Tab-selected RPL menu rows across soft-key actions and key releases.
// A different menu starts with its own initial row; stack arrows retain their plane.
template<class Interface,class StackView>
bool pico_stack_menu_key(Interface &ui,StackView &stack,int key,bool repeating,bool alpha)
{
    static bool plain_release=false;
    bool active=stack.interactive!=0;unsigned plane=ui.shift_plane();
    auto menu=ui.menu();
    bool navigate=active&&(key==18||key==23);
    bool leave=(active&&key==13)||key==33; // Esc always cancels, even on a shifted menu row.
    bool plain=!active&&menu&&key>0&&key!=28&&!(key>=38&&key<=43);
    if(key>0)plain_release=plain;
    bool unshifted=plain||(key==0&&plain_release);
    if(navigate||leave||unshifted)ui.clear_shift();
    bool consumed=ui.key(key,repeating,alpha);
    if((active&&stack.interactive&&(key==0||navigate)) ||
       (!active && menu==ui.menu() && (unshifted || key==0 || (key>=38&&key<=43))))
    {
        ui.shift_plane(plane);
    }
    if((leave&&(active||!menu||menu!=ui.menu())) ||
       (active&&!stack.interactive))ui.clear_shift();
    if(key==0)plain_release=false;
    return consumed;
}
