// Execute every settings soft key through the production press/release controller.
struct AdvancedSettingProbe : object
{
    static int prepare(id command, long long &expected)
    {
        switch(command)
        {
#define ID(i)
#define FLAG(Enable,Disable) \
 case ID_##Enable: expected=!Settings.Enable();return 1; \
 case ID_##Disable: expected=!Settings.Disable();return 1;
#define SETTING(Name,Low,High,Init) \
 case ID_##Name: expected=static_cast<long long>(Init);expected+=(expected<static_cast<long long>(High)?1:-1);rt.push(integer::make(expected));return 2;
#define SETTING_VALUE(Name,Alias,Base,Value) \
 case ID_##Name: expected=Value;return 3;
#include "ids.tbl"
        default:return 0;
        }
    }
    static long long actual(id command)
    {
        switch(command)
        {
#define ID(i)
#define FLAG(Enable,Disable) \
 case ID_##Enable:return Settings.Enable();case ID_##Disable:return Settings.Disable();
#define SETTING(Name,Low,High,Init) case ID_##Name:return static_cast<long long>(Settings.Name());
#define SETTING_VALUE(Name,Alias,Base,Value) case ID_##Name:return Settings.Base();
#include "ids.tbl"
        default:return -999;
        }
    }
};
static void advanced_settings_cases()
{
    auto press=[](int key){pico_stack_menu_key(ui,Stack,key,false,false);pico_stack_menu_key(ui,Stack,0,false,false);};
    auto reset=[](){ui.clear_menu();rt.reset();load_saved_keymap();Settings=settings();ui.clear_shift();ui.alpha_plane(0);};
    struct Entry {object::id menu,command;unsigned page,plane,column;};
    std::vector<Entry> entries;
    for(auto id:{object::ID_ModesMenu,object::ID_DisplayModesMenu,object::ID_SeparatorModesMenu,
                 object::ID_UserInterfaceModesMenu,object::ID_MathModesMenu,object::ID_UserModeMenu,object::ID_AnglesMenu})
    {
        reset();ui.menu(menu_p(object::static_object(id)));
        unsigned pages=ui.pages();
        for(unsigned page=0;page<pages;++page)
        {
            ui.page(page);
            for(unsigned plane=0;plane<ui.menu_planes();++plane)
            {
                ui.shift_plane(plane);
                for(unsigned column=0;column<6;++column)
                    if(auto item=ui.object_for_key(38+column))
                        entries.push_back({id,item->type(),page,plane,column});
            }
        }
    }
    FILE *report=fopen("../../reports/port196-advanced-actions.tsv","w");
    fprintf(report,"Menu\tPage\tRow\tColumn\tCommand\tCheck\n");unsigned settings_count=0;
    for(auto e:entries)
    {
        reset();ui.menu(menu_p(object::static_object(e.menu)),e.page);
        for(unsigned p=0;p<e.plane;++p)press(28);
        if(ui.object_for_key(38+e.column)->type()!=e.command){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
        long long expected=0;int kind=AdvancedSettingProbe::prepare(e.command,expected);
        if(e.command>=object::ID_Sig&&e.command<=object::ID_Eng)rt.push(integer::make(4));
        const char *check="navigation/command";
        if(kind)
        {
            press(38+e.column);
            if(rt.error()||AdvancedSettingProbe::actual(e.command)!=expected)
            {fprintf(stderr,"FAIL: advanced setting %s row %u col %u: %s expected %lld actual %lld\n",object::name(e.command),e.plane,e.column,rt.error()?reinterpret_cast<const char *>(rt.error()):"value",expected,AdvancedSettingProbe::actual(e.command));std::exit(1);}
            long long actual=AdvancedSettingProbe::actual(e.command);
            pico_stack_menu_key(ui,Stack,0,false,false);
            if(AdvancedSettingProbe::actual(e.command)!=actual){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
            check="setting value + duplicate release";++settings_count;
            if(kind==2)
            {
                // Empty arguments must fail cleanly and keep the previous setting.
                while(rt.depth())rt.drop();press(38+e.column);
                if(!rt.error()||AdvancedSettingProbe::actual(e.command)!=actual){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
                press(33);
                if(rt.error()||ui.shift_plane()!=e.plane){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
            }
        }
        else if(object::is_menu(e.command))
        {
            press(38+e.column);
            if(rt.error()||!ui.menu()||ui.menu()->type()!=e.command||ui.shift_plane()){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
            check="submenu + initial row";
        }
        else if(e.command==object::ID_MenuNextPage||e.command==object::ID_MenuPreviousPage)
        {
            unsigned pages=ui.pages();unsigned expected_page=(e.page+pages+(e.command==object::ID_MenuNextPage?1:-1))%pages;
            press(38+e.column);
            if(rt.error()||ui.page()!=expected_page||ui.shift_plane()!=e.plane)std::abort();
            check="page navigation + selected row";
        }
        else if(e.command==object::ID_Modes)
        {
            Settings.Precision(39);
            press(38+e.column);if(rt.error()||!rt.top()||!rt.top()->as_program()){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
            Settings.Precision(60);object::static_object(object::ID_Eval)->evaluate();
            if(rt.error()||Settings.Precision()!=39){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
            check="nondefault settings program evaluated";
        }
        else if(e.command==object::ID_ResetModes)
        {
            Settings.Precision(39);press(38+e.column);
            if(rt.error()||Settings.Precision()!=settings().Precision()){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
            check="factory settings restored";
        }
        else if(e.command==object::ID_SystemSetup)
        {
            force({27});auto before=visits["desktop.native_settings.choose1"];
            press(38+e.column);if(rt.error()||visits["desktop.native_settings.choose1"]<=before){fprintf(stderr,"FAIL: PicoCalc System routing\n");std::exit(1);}
            check="PicoCalc settings return";
        }
        else
        {
            // Stack-consuming auxiliary commands must fail explicitly on missing input.
            press(38+e.column);
            if(rt.error()){press(33);if(rt.error()){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}check="missing-argument error + Esc";}
            else check="no-argument command";
        }
        fprintf(report,"%s\t%u\t%u\t%u\t%s\t%s\n",object::name(e.menu),e.page+1,e.plane+1,e.column+1,object::name(e.command),check);fflush(report);
    }
    fclose(report);
    auto invoke=[&](object::id command)
    {
        for(auto e:entries)if(e.command==command)
        {
            ui.clear_menu();ui.clear_shift();ui.menu(menu_p(object::static_object(e.menu)),e.page);
            for(unsigned p=0;p<e.plane;++p)press(28);
            press(38+e.column);
            if(rt.error()){fprintf(stderr,"FAIL: valid auxiliary %s\n",object::name(command));std::exit(1);}
            return;
        }
        std::abort();
    };
    reset();rt.push(text::make("Custom header"));invoke(object::ID_Header);
    if(rt.depth()||!rt.variables(0)->recall(object::static_object(object::ID_Header)))std::abort();
    reset();rt.push(integer::make(42));rt.push(integer::make(11));invoke(object::ID_AssignKey);
    unsigned assigned=ui.key_for_position(integer::make(11));
    if(!ui.assigned(assigned)||ui.assigned(assigned)->as_int32(0,false)!=42)std::abort();
    invoke(object::ID_RecallKeys);object_g keys=rt.pop();if(!keys||!keys->as_array_or_list())std::abort();
    rt.push(integer::make(11));invoke(object::ID_DeleteKeys);if(ui.assigned(assigned))std::abort();
    rt.drop(rt.depth());rt.push(keys);invoke(object::ID_StoreKeys);
    if(!ui.assigned(assigned)||ui.assigned(assigned)->as_int32(0,false)!=42)std::abort();
    object_g map=rt.variables(0)->recall(object::static_object(object::ID_KeyMap));
    if(!map)std::abort();rt.push(map);invoke(object::ID_KeyMap);if(rt.depth())std::abort();
    reset();rt.push(integer::make(3));rt.push(integer::make(4));invoke(object::ID_hypot);
    if(rt.depth()!=1||rt.top()->as_int32(0,false)!=5)std::abort();
    reset();rt.push(integer::make(1));invoke(object::ID_DegreesToRadians);
    if(rt.depth()!=1||rt.top()->is_zero(false))std::abort();
    puts("PASS: advanced Header storage, Assign/Recall/Delete/Store/KeyMap round-trip and angle functions with valid inputs");
    for(unsigned plane=0;plane<3;++plane)
    {
        reset();ui.menu(menu_p(object::static_object(object::ID_ModesMenu)));
        for(unsigned p=0;p<plane;++p)press(28);
        rt.push(integer::make(2));rt.push(integer::make(3));press(KEY_ADD); // +
        if(rt.error()||rt.depth()!=1||rt.top()->as_int32(0,false)!=5||ui.shift_plane()!=plane){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
        press(13);if(rt.error()||rt.depth()!=2||ui.alpha_plane()||ui.showing_help()||ui.showing_graphics()){fprintf(stderr,"FAIL: advanced audit assertion line %d\n",__LINE__);std::exit(1);}
    }
    for(int theme=0;theme<int(pico_themes::count);++theme)
    {
        reset();host_theme=theme;apply_theme(theme);host_variant="advanced-modes";
        ui.menu(menu_p(object::static_object(object::ID_ModesMenu)));press(28);press(28);
        calculator_frame("calculator.softmenus","Advanced settings and snapshot hint");
        press(38);press(13);host_variant="advanced-snapshot-enter";
        calculator_frame("calculator.stack","Settings snapshot after ordinary Enter");
        host_variant="advanced-large-show";key_push(KEY_EXIT);key_push(0);
        object::static_object(object::ID_Show)->evaluate();key_pop_all();
        if(rt.error())
        {
            calculator_frame("calculator.stack","Large settings program Show limit");
            press(KEY_EXIT);
            if(rt.error())std::abort();
        }
        host_variant="advanced-show-return";rt.push(text::make("Settings"));
        key_push(KEY_EXIT);key_push(0);object::static_object(object::ID_Show)->evaluate();key_pop_all();rt.drop();
        if(rt.error()||ui.showing_graphics())std::abort();
        calculator_frame("calculator.stack","Settings snapshot after full-screen Show and Esc");
    }
    for(auto id:{object::ID_ModesMenu,object::ID_DisplayModesMenu,object::ID_SeparatorModesMenu,
                 object::ID_UserInterfaceModesMenu,object::ID_MathModesMenu,object::ID_UserModeMenu,object::ID_AnglesMenu})
    {
        reset();ui.menu(menu_p(object::static_object(id)));unsigned pages=ui.pages();
        for(unsigned page=0;page<pages;++page)for(int theme=0;theme<int(pico_themes::count);++theme)
        {
            reset();host_theme=theme;apply_theme(theme);ui.menu(menu_p(object::static_object(id)),page);
            host_variant=std::string("advanced-audit-")+reinterpret_cast<const char *>(object::name(id))+"-page"+std::to_string(page+1);
            calculator_frame("calculator.softmenus","Advanced RPL settings page audit");
        }
    }
    reset();host_theme=0;apply_theme(0);host_variant="normal";
    printf("PASS: %zu advanced menu slots; %u setting outcomes, argument errors, duplicate release, snapshot restore, System and ordinary Enter/arithmetic\n",entries.size(),settings_count);
}
