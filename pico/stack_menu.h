#pragma once
#include "command.h"
#include "variables.h"
#include "text.h"
// Keep Tab-selected RPL menu rows across soft-key actions and key releases.
// Submenus start on their initial row; Back restores the parent row.
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
    if((leave&&(active||!menu||!ui.menu())) ||
       (active&&!stack.interactive))ui.clear_shift();
    if(key==0)plain_release=false;
    return consumed;
}

// Only a complete pair of numeric literals is an inline power shortcut.
// Programs, variable names and quoted algebra retain the engine's syntax.
inline bool pico_calculator_numeric_literal(const byte *text,size_t first,size_t last)
{
    while(first<last && text[first]==' ')++first;
    while(last>first && text[last-1]==' ')--last;
    if(first<last && (text[first]=='-'||text[first]=='+'))++first;
    bool digits=false;
    while(first<last && text[first]>='0' && text[first]<='9'){digits=true;++first;}
    if(first<last && (text[first]=='.'||text[first]==','))
    {
        ++first;
        while(first<last && text[first]>='0' && text[first]<='9'){digits=true;++first;}
    }
    if(!digits)return false;
    if(first<last && (text[first]=='e'||text[first]=='E'))
    {
        ++first;
        if(first<last && (text[first]=='-'||text[first]=='+'))++first;
        size_t start=first;
        while(first<last && text[first]>='0' && text[first]<='9')++first;
        if(first==start)return false;
    }
    return first==last;
}

inline bool pico_calculator_grouped_formula(const byte *text,size_t first,size_t last)
{
    while(first<last && text[first]==' ')++first;
    while(last>first && text[last-1]==' ')--last;
    if(first==last || text[first]!='(')return false;
    for(size_t n=first;n<last;++n)
    {
        if(text[n]==';' || text[n]=='\'')return false;
        if(n+3<=last && !memcmp(text+n,"ⅈ",3))return false;
    }
    // Keep classic (real real) complex literals in their original syntax.
    if(last>first+2 && text[last-1]==')')
        for(size_t split=first+1;split<last-1;++split)
            if(text[split]==' ' && pico_calculator_numeric_literal(text,first+1,split) &&
               pico_calculator_numeric_literal(text,split+1,last-1))return false;
    return true;
}

// Printable PicoCalc keys share this path with the production-controller probe.
// With two stack operands, minus subtracts immediately. Otherwise a leading
// minus starts a signed literal; operators after a number retain normal action.
template<class Interface,class StackView>
bool pico_calculator_text_key(Interface &ui,StackView &stack,unsigned c,bool repeating)
{
    if(rt.error())
    {
        if(Settings.NeedToClearErrors())
            return pico_stack_menu_key(ui,stack,KB_ADD,repeating,false);
        rt.clear_error();
        pico_stack_menu_key(ui,stack,0,false,false);
    }
    if(ui.in_input())
    {
        ui.insert(c,Interface::DIRECT,false);
        return true;
    }
    unsigned cursor=ui.cursor_position();
    const auto *editor=rt.editor();
    if(rt.editing() && cursor<rt.editing() && !stack.interactive &&
       !ui.showing_help() && !ui.showing_graphics() &&
       (c=='+'||c=='-'||c=='*'||c=='/'||c=='^'||c=='!'||c=='?'))
    {
        ui.insert(c,Interface::DIRECT,false);
        return true;
    }
    if(pico_calculator_grouped_formula(editor,0,rt.editing()) &&
       (c=='+'||c=='-'||c=='*'||c=='/'||c=='^'||c=='!'||c=='?'))
    {
        ui.insert(c,Interface::DIRECT,false);
        return true;
    }
    bool power_sign=(c=='-'||c=='+') && cursor && cursor<=rt.editing() && editor[cursor-1]=='^';
    bool exponent_sign=(c=='-'||c=='+') && cursor>=2 && cursor<=rt.editing() &&
        (editor[cursor-1]=='e'||editor[cursor-1]=='E') &&
        editor[cursor-2]>='0' && editor[cursor-2]<='9';
    if(((c=='-' && !rt.editing() && rt.depth()<2)||exponent_sign||power_sign) && !stack.interactive &&
       !ui.showing_help() && !ui.showing_graphics())
    {
        ui.insert(c,Interface::DIRECT,false);
        return true;
    }
    if(c=='^' && !stack.interactive && !ui.showing_help() && !ui.showing_graphics())
    {
        size_t length=rt.editing();bool ready=!length;
        if(length && rt.depth() && pico_calculator_numeric_literal(editor,0,length))ready=true;
        for(size_t split=0;!ready && split<length;++split)
            if(editor[split]==' ' && pico_calculator_numeric_literal(editor,0,split) &&
               pico_calculator_numeric_literal(editor,split+1,length))ready=true;
        // The engine's fourth physical key is its ordinary power key.
        if(ready)return pico_stack_menu_key(ui,stack,KB_D,repeating,false);
    }
    if((c=='!'||c=='?') && (!rt.editing() ||
       pico_calculator_numeric_literal(editor,0,rt.editing())) && !stack.interactive &&
       !ui.showing_help() && !ui.showing_graphics() && !ui.in_input())
    {
        ui.insert(c,Interface::DIRECT,false);
        return pico_stack_menu_key(ui,stack,13,repeating,false);
    }
    int operation=c=='+'?KB_ADD:c=='-'?KB_SUB:c=='*'?KB_MUL:c=='/'?KB_DIV:0;
    if(operation)return pico_stack_menu_key(ui,stack,operation,repeating,false);
    ui.insert(c,Interface::DIRECT,false);
    return true;
}

// Recognize the convenient imaginary-unit spelling in a calculator entry.
// Keep RPL programs, quoted algebra and iterator names in longer lines intact.
template<class Interface>
bool pico_calculator_prepare_entry(Interface &ui)
{
    if(ui.showing_help()||ui.showing_graphics()||ui.in_input())return true;
    size_t length=rt.editing(),first=0,last=length;
    const auto *text=rt.editor();
    while(first<last && text[first]==' ')++first;
    while(last>first && text[last-1]==' ')--last;
    if(pico_calculator_grouped_formula(text,first,last))
    {
        if(ui.insert(last,utf8("' EVAL"),6)!=6)return false;
        if(ui.insert(first,U'\'')!=1)return false;
        ui.cursor_position(last+7);
        ui.update_mode();
        return true;
    }
    for(size_t power=first;power<last;++power)
    {
        if(text[power]=='^' &&
           pico_calculator_numeric_literal(text,first,power) &&
           pico_calculator_numeric_literal(text,power+1,last))
        {
            if(ui.insert(last,utf8("' EVAL"),6)!=6)return false;
            if(ui.insert(first,U'\'')!=1)return false;
            ui.cursor_position(last+7);
            ui.update_mode();
            return true;
        }
    }
    if(first==last || (text[first]!='i' && text[first]!='I'))return true;
    size_t suffix=first+1;
    while(suffix<last && text[suffix]==' ')++suffix;
    if(suffix<last)
    {
        size_t count=last-suffix;
        auto cmd=command::lookup(utf8(text+suffix),count);
        if(cmd!=object::ID_fact || count!=last-suffix)return true;
    }
    symbol_g name=symbol::make("i");
    if(!name)return false;
    if(directory::recall_all(name,false))return true;
    unsigned cursor=ui.cursor_position();
    size_t added=ui.insert(first+1,U'ⅈ');
    if(!added)return false;
    ui.remove(first,1);
    if(cursor>first)cursor+=added-1;
    ui.cursor_position(cursor);
    return true;
}
template<class Interface,class StackView>
bool pico_calculator_key(Interface &ui,StackView &stack,int key,bool repeating,bool alpha)
{
    if(key!=13 || rt.error())
        return pico_stack_menu_key(ui,stack,key,repeating,alpha);
    size_t length=rt.editing(),first=0;
    while(first<length && rt.editor()[first]==' ')++first;
    bool transformed=!ui.in_input() &&
        pico_calculator_grouped_formula(rt.editor(),0,length);
    for(size_t power=first;!transformed && !ui.in_input() && power<length;++power)
        transformed=rt.editor()[power]=='^' &&
            pico_calculator_numeric_literal(rt.editor(),first,power) &&
            pico_calculator_numeric_literal(rt.editor(),power+1,length);
    text_g original=transformed?text::make(rt.editor(),length):nullptr;
    if(transformed && !original)return true;
    unsigned original_cursor=ui.cursor_position();
    size_t original_depth=rt.depth();
    bool prepared=pico_calculator_prepare_entry(ui);
    bool consumed=prepared?pico_stack_menu_key(ui,stack,key,repeating,alpha):true;
    if(original && rt.error())
    {
        auto position=[&](unsigned pos)
        {
            if(pos>first)--pos;
            return pos>length?unsigned(length):pos;
        };
        unsigned cursor=rt.editing()?position(ui.cursor_position()):original_cursor;
        size_t error_length=rt.editing()?rt.source_length():0;
        unsigned selection=error_length?unsigned(cursor+error_length):~0U;
        if(selection!=~0U && selection>length)selection=length;
        int unclosed=0;
        for(size_t n=0;n<length;++n)
        {
            if(original->value()[n]=='(')++unclosed;
            if(original->value()[n]==')')--unclosed;
        }
        if(unclosed>0){cursor=length;selection=~0U;}
        // Restore both user-visible text and error coordinates, not the adapter.
        error_save error;
        if(rt.depth()>original_depth)rt.drop(rt.depth()-original_depth);
        ui.clear_editor();
        if(rt.edit(original->value(),length)==length)
        {
            if(selection!=~0U)
            {
                ui.cursor_position(selection);
                ui.editor_select();
            }
            ui.cursor_position(cursor);
            ui.update_mode();
            error.source=rt.editor()+cursor;
            error.srclen=selection!=~0U && selection>cursor?selection-cursor:0;
        }
    }
    return consumed;
}
