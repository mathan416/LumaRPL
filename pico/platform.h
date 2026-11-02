#include <stddef.h>
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void pico_devices_init(void);
void pico_poll_background(void);
int pico_partition_fits(void);
void pico_mark_dirty(unsigned row);
void pico_display_refresh(const uint8_t *frame);
int pico_keyboard_poll(void);
int pico_keyboard_event(int *state);
void pico_keyboard_reset(void);
void pico_keyboard_transition(void);
typedef struct {uint32_t rows,refresh_us,input_us,cursor_us,cursor_rows,max_cursor_us;} pico_ui_performance;
void pico_ui_input_begin(void);
void pico_ui_cursor_complete(void);
void pico_ui_performance_snapshot(pico_ui_performance *);
int pico_battery_percent(void);
int pico_battery_charging(void);
void pico_backlight(int on);
void pico_return_to_loader(void);
int pico_read_key(int *, int *);
#ifdef __cplusplus
}
bool pico_ui_calculator_redraw(bool consume=true);
bool pico_handle_shortcut(int key);
bool pico_prepare_session_action();
void pico_load_startup_session();
void pico_setup_menu();
void pico_about_menu();
void pico_settings_menu();
void pico_reference_find();
void pico_show_command_reference(const char *,size_t);
void pico_open_calculator_menu(int menu);
bool pico_prepare_exit();
void pico_program_browser();
void pico_graph_menu();
void pico_workbench_menu();
#endif

void pico_data_menu();

void pico_forms_menu();

void pico_notebooks_menu();
void pico_conversions_menu();
void pico_wifi_menu();

void pico_path_menu();
void pico_ode_menu();

#ifdef __cplusplus
#include "timezone.h"
pico_timezone pico_get_timezone();
void pico_set_timezone(const pico_timezone &);
void pico_clock_sync(int64_t utc);
int64_t pico_clock_utc();
#endif
