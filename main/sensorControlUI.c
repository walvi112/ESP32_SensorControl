#include "sensorControlUI.h"
#include "sensorControlDevice.h"
#include "img_background.h"
#include "esp_log.h"
#include <sys/time.h>
#include <time.h>
#include "DHTSensor.h"

#define MENU_CONTENTS    2

static void screen1_init(void);
static void screen2_init(void);
static void screen3_init(void);

static void screen1_event(lv_event_t *e);
static void imu_button_pressed(lv_event_t *e);
static void temperature_button_pressed(lv_event_t *e);
static void screen2_event(lv_event_t *e);
static void screen3_event(lv_event_t *e);
static void menu_back_event(lv_event_t *e);
static void roller_event(lv_event_t *e);
static void calendar_event(lv_event_t *e);
static void setting_event(lv_event_t *e);

static void get_time_cb(lv_timer_t *timer);

static void imu_create(lv_obj_t *parent, lv_group_t *group);
static void temperature_create(lv_obj_t *parent, lv_group_t *group);
static lv_obj_t *calendar_create(lv_obj_t *parent, lv_group_t *group);
static lv_obj_t *time_set_create(lv_obj_t *parent, lv_group_t *group);
static void setting_confirm_msgbox_create(void);

static void window_delete_event_cb(lv_event_t *e);
static void remove_padding(lv_obj_t *obj);
static void group_change(lv_group_t *new_group, uint32_t index);

static const lv_font_t *font_large;
static const lv_font_t *font_normal;
static const lv_font_t *font_clock;

static lv_style_t style_cont;
static lv_style_t style_sensor_cont;
static lv_style_t style_text_muted;
static lv_style_t style_clock;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;
static lv_style_t style_button;

static lv_obj_t *screen1;
static lv_obj_t *screen2;
static lv_obj_t *screen3;

static lv_obj_t *screen1_date_label;
static lv_obj_t *screen1_time_label;
static lv_obj_t *screen2_date_label;
static lv_obj_t *screen2_time_label;

static lv_obj_t *temperature_val_t;
static lv_obj_t *humidity_val_t;

static lv_group_t *group_prev;
static lv_group_t *group_screen1;
static lv_group_t *group_screen2;
static lv_group_t *group_screen3;
static lv_group_t *group_msg;
static lv_group_t *group_menu[MENU_CONTENTS];

static lv_obj_t *menu_root_page;
static lv_obj_t *mbox1;

static lv_timer_t *lv_time_timer;

static uint8_t menu_index = 0;

static struct timeval now;
static struct tm *time_struct;
static char now_time_buff[6];
static char now_date_buff[28];

extern const char *TAG;

extern DHT11Struct dht11;

void sensorControlInit(void)
{
    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;
    font_clock = LV_FONT_DEFAULT;

    #if LV_FONT_MONTSERRAT_14
        font_large = &lv_font_montserrat_14;
    #endif
    #if LV_FONT_MONTSERRAT_12
        font_normal = &lv_font_montserrat_12;
    #endif
    #if LV_FONT_MONTSERRAT_40
        font_clock = &lv_font_montserrat_40;
    #endif

    lv_obj_add_event_cb(lv_screen_active(), window_delete_event_cb, LV_EVENT_DELETE, NULL);

    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_ORANGE), LV_THEME_DEFAULT_DARK, font_normal);

    lv_style_init(&style_cont);
    lv_style_set_bg_opa(&style_cont, LV_OPA_TRANSP);
    lv_style_set_border_opa(&style_cont, LV_OPA_TRANSP);

    lv_style_init(&style_sensor_cont);
    lv_style_set_bg_opa(&style_sensor_cont, 100);
    lv_style_set_border_opa(&style_sensor_cont, 100);

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

    lv_style_init(&style_clock);
    lv_style_set_text_font(&style_clock, font_clock);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

    lv_style_init(&style_button);
    lv_style_set_bg_color(&style_button, lv_palette_main(LV_PALETTE_ORANGE));
    lv_style_set_outline_opa(&style_button, LV_OPA_TRANSP);

    now.tv_sec = 1733504400;   //init time before adding RTC module
    settimeofday(&now, NULL);
    lv_time_timer = lv_timer_create(get_time_cb, 1000, NULL);
    lv_timer_ready(lv_time_timer);

    screen1_init();
    screen2_init();
    screen3_init();

    group_change(group_screen2, 0);
    lv_screen_load(screen2);
}

//Screen init functions

static void screen1_init(void)
{
    screen1 = lv_obj_create(NULL);
    group_screen1 = lv_group_create();
    lv_obj_set_style_bg_image_src(screen1, &img_background, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_layout(screen1, LV_LAYOUT_GRID);
    static int32_t main_column_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t main_row_dsc[] = {
            LV_GRID_CONTENT,  /*Date*/
            LV_GRID_CONTENT,  /*Time*/
            10,
            LV_GRID_FR(1),  /*Sensor*/
            LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(screen1, main_column_dsc, main_row_dsc);

    screen1_date_label = lv_label_create(screen1);
    lv_label_set_text_static(screen1_date_label, now_date_buff);
    lv_obj_add_style(screen1_date_label, &style_title, 0);
    lv_obj_set_style_pad_top(screen1_date_label, 10 , 0);


    screen1_time_label = lv_label_create(screen1);
    lv_label_set_text(screen1_time_label, now_time_buff);
    lv_obj_add_style(screen1_time_label, &style_title, 0);

    lv_obj_t *sensor_cont = lv_obj_create(screen1);
    lv_obj_add_style(sensor_cont, &style_cont, 0);
    remove_padding(sensor_cont);
    lv_obj_set_flex_flow(sensor_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(sensor_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_set_grid_cell(screen1_date_label, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(screen1_time_label, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(sensor_cont, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_STRETCH, 3, 1);

    lv_obj_t *imu_cont = lv_obj_create(sensor_cont);
    lv_obj_add_style(imu_cont, &style_sensor_cont, 0);
    imu_create(imu_cont, group_screen1);
    lv_obj_set_size(imu_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t *temperature_cont = lv_obj_create(sensor_cont);
    lv_obj_add_style(temperature_cont, &style_sensor_cont, 0);
    temperature_create(temperature_cont, group_screen1);
    lv_obj_set_size(temperature_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_add_event_cb(screen1, screen1_event, LV_EVENT_ALL, NULL);
}

static void screen2_init(void)
{
    screen2 = lv_obj_create(NULL);
    group_screen2 = lv_group_create();
    lv_group_add_obj(group_screen2, screen2);
    lv_obj_set_style_bg_image_src(screen2, &img_background, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_layout(screen2, LV_LAYOUT_FLEX);
    lv_obj_remove_flag(screen2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(screen2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    screen2_date_label = lv_label_create(screen2);
    lv_label_set_text_static(screen2_date_label, now_date_buff);
    lv_obj_add_style(screen2_date_label, &style_title, 0);

    screen2_time_label = lv_label_create(screen2);
    lv_label_set_text_static(screen2_time_label, now_time_buff);
    lv_obj_add_style(screen2_time_label, &style_clock, 0);

    lv_obj_add_event_cb(screen2, screen2_event, LV_EVENT_ALL, NULL);
}

static void screen3_init(void)
{
    screen3 = lv_obj_create(NULL);
    group_screen3 = lv_group_create();
    lv_obj_set_style_bg_image_src(screen3, &img_background, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *cont;
    lv_obj_t *label;
    lv_obj_t *section;

    lv_obj_t *menu = lv_menu_create(screen3);
    lv_obj_set_style_bg_opa(menu, LV_OPA_TRANSP , 0);
    
    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
    lv_obj_add_event_cb(menu, menu_back_event, LV_EVENT_CLICKED, menu);
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);

    lv_obj_t *set_time_page = lv_menu_page_create(menu, NULL);
    group_menu[MENU_SET_TIME] = lv_group_create();
    lv_obj_remove_flag(set_time_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_hor(set_time_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(set_time_page);
    lv_obj_set_layout(set_time_page, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(set_time_page, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(set_time_page, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    section = lv_menu_section_create(set_time_page);
    lv_obj_set_style_bg_opa(section, LV_OPA_TRANSP, 0);  
    time_set_create(section, group_menu[MENU_SET_TIME]);

    lv_obj_t *set_date_page = lv_menu_page_create(menu, NULL);
    group_menu[MENU_SET_DATE] = lv_group_create();
    lv_obj_remove_flag(set_date_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_hor(set_date_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(set_date_page);
    section = lv_menu_section_create(set_date_page);
    calendar_create(section, group_menu[MENU_SET_DATE]);


    menu_root_page = lv_menu_page_create(menu, "Settings");
    lv_obj_set_style_pad_hor(menu_root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);

    section = lv_menu_section_create(menu_root_page);

    cont = lv_menu_cont_create(section);
    lv_group_add_obj(group_screen3, cont);
    lv_obj_add_event_cb(cont, screen3_event, LV_EVENT_KEY, NULL);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Set time");
    lv_menu_set_load_page_event(menu, cont, set_time_page);

    cont = lv_menu_cont_create(section);
    lv_group_add_obj(group_screen3, cont);
    lv_obj_add_event_cb(cont, screen3_event, LV_EVENT_KEY, NULL);
    label = lv_label_create(cont);
    lv_obj_t *my_label = label;
    lv_label_set_text(my_label, "Set date");
    lv_menu_set_load_page_event(menu, cont, set_date_page);

    lv_menu_set_sidebar_page(menu, menu_root_page);
    group_msg = lv_group_create();
}   

//Widget create functions

static void imu_create(lv_obj_t *parent, lv_group_t *group)
{
    lv_obj_t *name = lv_label_create(parent);
    lv_label_set_text(name, "IMU");
    lv_obj_add_style(name, &style_title, 0);

    uint32_t x_val = 5;
    uint32_t y_val = 5;
    uint32_t z_val = 5;

    lv_obj_t *x_val_t = lv_label_create(parent);
    lv_label_set_text_fmt(x_val_t, "X = %"LV_PRIu32, x_val);
    lv_obj_t *y_val_t = lv_label_create(parent);
    lv_label_set_text_fmt(y_val_t, "Y = %"LV_PRIu32, y_val);
    lv_obj_t *z_val_t = lv_label_create(parent);
    lv_label_set_text_fmt(z_val_t, "Z = %"LV_PRIu32, z_val);

    lv_obj_t *btn1 = lv_button_create(parent);
    lv_group_add_obj(group, btn1);
    lv_obj_add_style(btn1, &style_button, LV_STATE_FOCUS_KEY);
    lv_obj_add_event_cb(btn1, screen1_event, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(btn1, imu_button_pressed, LV_EVENT_PRESSED, NULL);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "Reset");
    lv_obj_center(label);

    static int32_t main_column_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t main_row_dsc[] = {
            LV_GRID_CONTENT,  /*Name*/
            LV_GRID_CONTENT,  /*X*/
            LV_GRID_CONTENT,  /*Y*/
            LV_GRID_CONTENT,  /*Z*/
            LV_GRID_TEMPLATE_LAST
    };

    lv_obj_set_grid_dsc_array(parent, main_column_dsc, main_row_dsc);
    lv_obj_set_grid_cell(name, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(x_val_t, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_set_grid_cell(y_val_t, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 2, 1);
    lv_obj_set_grid_cell(z_val_t, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 3, 1);
    lv_obj_set_grid_cell(btn1, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 3);

}

static void temperature_create(lv_obj_t *parent, lv_group_t *group)
{
    lv_obj_t *name = lv_label_create(parent);
    lv_label_set_text(name, "Temperature");
    lv_obj_add_style(name, &style_title, 0);

    temperature_val_t = lv_label_create(parent);
    lv_label_set_text_fmt(temperature_val_t, "T = %"LV_PRIu32" C", (uint32_t) dht11.temperature);
    humidity_val_t = lv_label_create(parent);
    lv_label_set_text_fmt(humidity_val_t, "H = %"LV_PRIu32" %%", (uint32_t) dht11.humidity);

    lv_obj_t *btn1 = lv_button_create(parent);
    lv_group_add_obj(group, btn1);
    lv_obj_add_style(btn1, &style_button, LV_STATE_FOCUS_KEY);
    lv_obj_add_event_cb(btn1, screen1_event, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(btn1, temperature_button_pressed, LV_EVENT_PRESSED, NULL);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "Reset");
    lv_obj_center(label);

    static int32_t main_column_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t main_row_dsc[] = {
            LV_GRID_CONTENT,  /*Name*/
            LV_GRID_CONTENT,  /*T*/
            LV_GRID_CONTENT,  /*H*/
            LV_GRID_TEMPLATE_LAST
    };

    lv_obj_set_grid_dsc_array(parent, main_column_dsc, main_row_dsc);
    lv_obj_set_grid_cell(name, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(temperature_val_t, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_set_grid_cell(humidity_val_t, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 2, 1);
    lv_obj_set_grid_cell(btn1, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 2);

}

static lv_obj_t *time_set_create(lv_obj_t *parent, lv_group_t *group)
{
    lv_obj_t *obj = lv_menu_cont_create(parent);
    lv_obj_set_layout(obj, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *hour_roller = lv_roller_create(obj);
    lv_obj_set_style_outline_color(hour_roller, lv_palette_main(LV_PALETTE_ORANGE), LV_STATE_FOCUS_KEY);
    lv_group_add_obj(group, hour_roller);
    lv_roller_set_options(hour_roller,  "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", LV_ROLLER_MODE_INFINITE);
    lv_obj_add_event_cb(hour_roller, roller_event, LV_EVENT_KEY, NULL);

    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, ":");
    lv_obj_add_style(label, &style_clock, 0);

    lv_obj_t *min_roller = lv_roller_create(obj);
    lv_obj_set_style_outline_color(min_roller, lv_palette_main(LV_PALETTE_ORANGE), LV_STATE_FOCUS_KEY);
    lv_group_add_obj(group, min_roller);
    lv_roller_set_options(min_roller,  "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59", LV_ROLLER_MODE_INFINITE);
    lv_obj_add_event_cb(min_roller, roller_event, LV_EVENT_KEY, NULL);

    return obj;
}

static lv_obj_t *calendar_create(lv_obj_t *parent, lv_group_t *group)
{
    lv_obj_t *obj = lv_menu_cont_create(parent);
    lv_obj_t  *calendar = lv_calendar_create(obj);
    lv_obj_t *header = lv_calendar_header_arrow_create(calendar);
    lv_obj_set_size(calendar, 185, 230);
    lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 27);

    lv_group_add_obj(group, lv_calendar_get_btnmatrix(calendar));
    lv_obj_add_event_cb(calendar, calendar_event, LV_EVENT_ALL, header);
    return obj;
}

static void setting_confirm_msgbox_create(void)
{
    mbox1 = lv_msgbox_create(NULL);
    lv_obj_set_style_outline_color(mbox1, lv_palette_main(LV_PALETTE_ORANGE), LV_STATE_DEFAULT);
    lv_msgbox_add_title(mbox1, "Apply change ?");
    lv_obj_t *btn;
    btn = lv_msgbox_add_footer_button(mbox1, "Apply");
    lv_group_add_obj(group_msg, btn);
    lv_obj_add_style(btn, &style_button, LV_STATE_FOCUS_KEY);
    lv_obj_add_event_cb(btn, setting_event, LV_EVENT_KEY, (void*) APPLY);
    btn = lv_msgbox_add_footer_button(mbox1, "Cancel");
    lv_group_add_obj(group_msg, btn);
    lv_obj_add_style(btn, &style_button, LV_STATE_FOCUS_KEY);
    lv_obj_add_event_cb(btn, setting_event, LV_EVENT_KEY, (void*) CANCEL);
    group_change(group_msg, 0);
}

//Support functions

static void remove_padding(lv_obj_t * obj)
{
    lv_obj_set_style_pad_left(obj, 0 , 0);
    lv_obj_set_style_pad_right(obj, 0 , 0);
    lv_obj_set_style_pad_top(obj, 0 , 0);
    lv_obj_set_style_pad_bottom(obj, 0 , 0);
}

static void window_delete_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_DELETE) {
        lv_style_reset(&style_cont);
        lv_style_reset(&style_sensor_cont);
        lv_style_reset(&style_text_muted);
        lv_style_reset(&style_clock);
        lv_style_reset(&style_title);
        lv_style_reset(&style_icon);
        lv_style_reset(&style_bullet);
        lv_style_reset(&style_button);
    }
}

static void group_change(lv_group_t *new_group, uint32_t index)
{
    group_prev = lv_indev_get_group(keypad);
    lv_indev_set_group(keypad, new_group);
    lv_obj_t *obj = lv_group_get_obj_by_index(new_group, index);
    lv_group_focus_obj(obj);
}

static void setting_apply(uint8_t menu_index)
{
    lv_timer_pause(lv_time_timer);
    switch(menu_index)
    {
        case MENU_SET_TIME:
            uint8_t new_hour = (uint8_t) lv_roller_get_selected(lv_group_get_obj_by_index(group_menu[MENU_SET_TIME], 0));
            uint8_t new_min = (uint8_t) lv_roller_get_selected(lv_group_get_obj_by_index(group_menu[MENU_SET_TIME], 1));
            time_struct->tm_hour = new_hour;
            time_struct->tm_min = new_min;
            now.tv_sec = mktime(time_struct);
            settimeofday(&now, NULL);
            break;
        case MENU_SET_DATE:
            lv_obj_t *calendar = lv_obj_get_parent(lv_group_get_obj_by_index(group_menu[MENU_SET_DATE], 0));
            lv_calendar_date_t today = {.day = 1, .month = 1, .year = 1900}; 
            lv_calendar_get_pressed_date(calendar, &today);
            time_struct->tm_year = today.year - 1900;
            time_struct->tm_mon = today.month - 1;
            time_struct->tm_mday = today.day;
            now.tv_sec = mktime(time_struct);
            settimeofday(&now, NULL);
            lv_obj_send_event(calendar, LV_EVENT_REFRESH, NULL);
            break;
    }
    lv_timer_resume(lv_time_timer);
    lv_timer_ready(lv_time_timer);
}

//Event cb function

static void screen1_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    switch(event_code)
    {   
        case LV_EVENT_KEY:
            switch(lv_event_get_key(e))
            {   
                case LV_KEY_UP:
                    group_change(group_screen2, 0);
                    lv_screen_load_anim(screen2, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, LVGL_ANIM_DELAY, 0, false);
                    break;
                case LV_KEY_DOWN:
                    group_change(group_screen2, 0);
                    lv_screen_load_anim(screen2, LV_SCR_LOAD_ANIM_MOVE_TOP, LVGL_ANIM_DELAY, 0, false);
                    break;
                case LV_KEY_LEFT:
                    lv_group_focus_prev(group_screen1);
                    break;  
                case LV_KEY_RIGHT:
                    lv_group_focus_next(group_screen1);
                    break;
            }
            break;
        case LV_EVENT_SCREEN_LOADED:
        case LV_EVENT_REFRESH:
            lv_label_set_text_static(screen1_date_label, now_date_buff);
            lv_label_set_text_static(screen1_time_label, now_time_buff);
            lv_label_set_text_fmt(temperature_val_t, "T = %"LV_PRIu32" C", (uint32_t) dht11.temperature);
            lv_label_set_text_fmt(humidity_val_t, "H = %"LV_PRIu32" %%", (uint32_t) dht11.humidity);
            break;
        default:
            break;

    }
}

static void screen2_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    switch(event_code)
    {
        case LV_EVENT_KEY:
            switch(lv_event_get_key(e))
            {
                case LV_KEY_UP:
                    group_change(group_screen1, 0);
                    lv_screen_load_anim(screen1, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, LVGL_ANIM_DELAY, 0, false);
                    break;
                case LV_KEY_DOWN:
                    group_change(group_screen1, 0);
                    lv_screen_load_anim(screen1, LV_SCR_LOAD_ANIM_MOVE_TOP, LVGL_ANIM_DELAY, 0, false);
                    break;
                case LV_KEY_ENTER:
                    lv_obj_send_event(lv_obj_get_parent(lv_group_get_obj_by_index(group_menu[1], 0)), LV_EVENT_REFRESH, NULL); //send refresh date event to calendar setting
                    group_change(group_screen3, 0);
                    lv_screen_load_anim(screen3, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
                    break;
            }
            break;
        case LV_EVENT_SCREEN_LOADED:
        case LV_EVENT_REFRESH:
            lv_label_set_text_static(screen2_date_label, now_date_buff);
            lv_label_set_text_static(screen2_time_label, now_time_buff);
            break;
        default:
            break;
    }
}

static void screen3_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target_obj(e);
    switch(event_code)
    {
        case LV_EVENT_KEY:
            switch(lv_event_get_key(e))
            {
                case LV_KEY_UP:
                    lv_group_focus_prev(group_screen3);
                    lv_obj_send_event(lv_group_get_focused(group_screen3), LV_EVENT_CLICKED, NULL);
                    break;
                case LV_KEY_DOWN:
                    lv_group_focus_next(group_screen3);
                    lv_obj_send_event(lv_group_get_focused(group_screen3), LV_EVENT_CLICKED, NULL);
                    break;
                case LV_KEY_LEFT:
                    lv_obj_send_event(target, LV_EVENT_DEFOCUSED, NULL);
                    group_change(group_screen2, 0);
                    lv_screen_load_anim(screen2, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, LVGL_ANIM_DELAY, 0, false);
                    break;
                case LV_KEY_ENTER:
                case LV_KEY_RIGHT:
                    lv_obj_send_event(lv_group_get_focused(group_screen3), LV_EVENT_PRESSED, NULL);
                    menu_index = lv_obj_get_index(target);
                    group_change(group_menu[menu_index], 0); 
                    break;
            }
            break;
        default:
            break;
    }
}

static void menu_back_event(lv_event_t *e)
{
    group_change(group_screen2, 0);
    lv_screen_load_anim(screen2, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, LVGL_ANIM_DELAY, 0, false);
}

static void imu_button_pressed(lv_event_t *e)
{
    ESP_LOGI(TAG, "IMU button pressed");
}

static void temperature_button_pressed(lv_event_t *e)
{
    lv_obj_send_event(lv_screen_active(), LV_EVENT_REFRESH, NULL);
}

static void roller_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);   
    lv_obj_t *target = lv_event_get_target_obj(e);
    lv_group_t *group = lv_obj_get_group(target);

    switch(event_code)
    {
        case LV_EVENT_KEY:
            switch(lv_event_get_key(e))
            {
                case LV_KEY_LEFT:
                    lv_group_focus_prev(group);
                    break;
                case LV_KEY_RIGHT:
                    lv_group_focus_next(group);
                    break;
                case LV_KEY_ENTER:
                    lv_obj_send_event(target, LV_EVENT_DEFOCUSED, NULL);
                    setting_confirm_msgbox_create();                    
                    break;
            }
            break;
        default:
            break;
    }
}

static void calendar_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);   
    lv_obj_t *target = lv_event_get_target_obj(e);
    lv_obj_t *header = lv_event_get_user_data(e);
    switch(event_code)
    {
        case LV_EVENT_KEY:
            switch(lv_event_get_key(e))
            {
                case LV_KEY_UP:
                    lv_obj_send_event(lv_obj_get_child(header, 0), LV_EVENT_CLICKED, NULL);
                    break;
                case LV_KEY_DOWN:
                    lv_obj_send_event(lv_obj_get_child(header, -1), LV_EVENT_CLICKED, NULL);
                    break;
                case LV_KEY_ENTER:
                    lv_obj_send_event(target, LV_EVENT_DEFOCUSED, NULL);
                    setting_confirm_msgbox_create();
                    break;
            }
            break;
        case LV_EVENT_REFRESH:
            lv_calendar_set_today_date(target, 1900 + time_struct->tm_year, 1 + time_struct->tm_mon, time_struct->tm_mday);
            lv_calendar_set_showed_date(target, 1900 + time_struct->tm_year, 1 + time_struct->tm_mon);
            break;
        default:
            break;
    }
}

static void setting_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);   
    switch(event_code)
    {
        case LV_EVENT_KEY:
            switch(lv_event_get_key(e))
            {
                case LV_KEY_UP:
                case LV_KEY_LEFT:
                    lv_group_focus_prev(group_msg);
                    break;
                case LV_KEY_DOWN:                    
                case LV_KEY_RIGHT:
                    lv_group_focus_next(group_msg);
                    break;
                case LV_KEY_ENTER:
                    lv_msgbox_close(mbox1);
                    lv_obj_send_event(lv_group_get_obj_by_index(group_screen3, menu_index), LV_EVENT_RELEASED, NULL);
                    group_change(group_screen3, menu_index);
                    if ((uintptr_t) lv_event_get_user_data(e))
                        setting_apply((uint8_t) menu_index);
                    break;
            }
            break;
        default:
            break;
    }
    
}

//Get time cb
static void get_time_cb(lv_timer_t *timer)
{
    gettimeofday(&now, NULL);
    time_struct = gmtime(&now.tv_sec);
    strftime(now_time_buff, 6, "%R", time_struct);
    strftime(now_date_buff, 28, "%A %B %e %G", time_struct);
    lv_obj_send_event(lv_screen_active(), LV_EVENT_REFRESH, NULL);
}
