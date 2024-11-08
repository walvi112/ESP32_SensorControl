#include "sensorControlUI.h"
#include "img_background.h"

static void screen1_init(void);
static void screen2_init(void);
static void screen3_init(void);

static void imu_create(lv_obj_t *parent);
static void temperature_create(lv_obj_t *parent);
static void calendar_create(lv_obj_t *parent);
static void time_set_create(lv_obj_t *parent);

static void window_delete_event_cb(lv_event_t *e);
static void remove_padding(lv_obj_t *obj);

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

static lv_obj_t *screen1;
static lv_obj_t *screen2;
static lv_obj_t *screen3;

static lv_obj_t *menu_root_page;

void sensorControl(void)
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

    lv_theme_default_init(NULL, lv_palette_lighten(LV_PALETTE_BLUE, 2), lv_palette_lighten(LV_PALETTE_ORANGE, 1), LV_THEME_DEFAULT_DARK, font_normal);

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


    screen1_init();
    screen2_init();
    screen3_init();

    lv_screen_load(screen3);
}

static void screen1_init(void)
{
    screen1 = lv_obj_create(NULL);
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

    lv_obj_t *date = lv_label_create(screen1);
    lv_label_set_text(date, "Monday 7th");
    lv_obj_add_style(date, &style_title, 0);
    lv_obj_set_style_pad_top(date, 10 , 0);


    lv_obj_t *time = lv_label_create(screen1);
    lv_label_set_text(time, "00:00");
    lv_obj_add_style(time, &style_title, 0);

    lv_obj_t *sensor_cont = lv_obj_create(screen1);
    lv_obj_add_style(sensor_cont, &style_cont, 0);
    remove_padding(sensor_cont);
    lv_obj_set_flex_flow(sensor_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(sensor_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_set_grid_cell(date, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(time, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(sensor_cont, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_STRETCH, 3, 1);

    lv_obj_t *imu_cont = lv_obj_create(sensor_cont);
    lv_obj_add_style(imu_cont, &style_sensor_cont, 0);
    imu_create(imu_cont);
    lv_obj_set_size(imu_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t *temperature_cont = lv_obj_create(sensor_cont);
    lv_obj_add_style(temperature_cont, &style_sensor_cont, 0);
    temperature_create(temperature_cont);
    lv_obj_set_size(temperature_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    
}

static void screen2_init(void)
{
    screen2 = lv_obj_create(NULL);
    lv_obj_set_style_bg_image_src(screen2, &img_background, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_layout(screen2, LV_LAYOUT_FLEX);
    lv_obj_remove_flag(screen2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(screen2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *date = lv_label_create(screen2);
    lv_label_set_text(date, "Monday 7th");
    lv_obj_add_style(date, &style_title, 0);

    lv_obj_t *time = lv_label_create(screen2);
    lv_label_set_text(time, "00:00");
    lv_obj_add_style(time, &style_clock, 0);
}

static void screen3_init(void)
{
    screen3 = lv_obj_create(NULL);
    lv_obj_set_style_bg_image_src(screen3, &img_background, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t *menu = lv_menu_create(screen3);
    
    lv_obj_t *cont;
    lv_obj_t *section;
    lv_obj_t *label;

    lv_obj_t *set_date_page = lv_menu_page_create(menu, NULL);
    section = lv_menu_section_create(set_date_page);
    calendar_create(section);

    lv_obj_t *set_time_page = lv_menu_page_create(menu, NULL);
    section = lv_menu_section_create(set_time_page);
    time_set_create(section);

    menu_root_page = lv_menu_page_create(menu, "Settings");
    cont = lv_menu_cont_create(menu_root_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Set date");
    lv_menu_set_load_page_event(menu, cont, set_date_page);

    cont = lv_menu_cont_create(menu_root_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Set time");
    lv_menu_set_load_page_event(menu, cont, set_time_page);

    lv_menu_set_page(menu, menu_root_page);
}   

static void time_set_create(lv_obj_t *parent)
{
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t *hour_roller = lv_roller_create(parent);
    lv_roller_set_options(hour_roller,  "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", LV_ROLLER_MODE_INFINITE);
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, ":");
    lv_obj_t *min_roller = lv_roller_create(parent);
    lv_roller_set_options(min_roller,  "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59", LV_ROLLER_MODE_INFINITE);

}

static void calendar_create(lv_obj_t *parent)
{
    lv_obj_t  * calendar = lv_calendar_create(parent);
    lv_obj_set_size(calendar, 185, 230);
    lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 27);
    lv_calendar_set_today_date(calendar, 2021, 02, 23);
    lv_calendar_set_showed_date(calendar, 2021, 02);
    // lv_obj_add_event_cb(calendar, event_handler, LV_EVENT_VALUE_CHANGED, NULL);


}

static void imu_create(lv_obj_t *parent)
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
    lv_obj_set_style_outline_color(btn1, lv_palette_main(LV_PALETTE_ORANGE), LV_STATE_FOCUS_KEY);
    // lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
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
    lv_obj_set_grid_cell(x_val_t, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(y_val_t, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(z_val_t, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(btn1, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 3);

}

static void temperature_create(lv_obj_t *parent)
{
    lv_obj_t *name = lv_label_create(parent);
    lv_label_set_text(name, "Temperature");
    lv_obj_add_style(name, &style_title, 0);

    uint32_t temperature_val = 5;
    uint32_t humidity_val = 5;

    lv_obj_t *temperature_val_t = lv_label_create(parent);
    lv_label_set_text_fmt(temperature_val_t, "T = %"LV_PRIu32, temperature_val);
    lv_obj_t *humidity_val_t = lv_label_create(parent);
    lv_label_set_text_fmt(humidity_val_t, "H = %"LV_PRIu32, humidity_val);

    lv_obj_t *btn1 = lv_button_create(parent);
    lv_obj_set_style_outline_color(btn1, lv_palette_main(LV_PALETTE_ORANGE), LV_STATE_FOCUS_KEY);
    // lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "Reset");
    lv_obj_center(label);

    static int32_t main_column_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t main_row_dsc[] = {
            LV_GRID_CONTENT,  /*Name*/
            LV_GRID_CONTENT,  /*X*/
            LV_GRID_CONTENT,  /*Y*/
            LV_GRID_TEMPLATE_LAST
    };

    lv_obj_set_grid_dsc_array(parent, main_column_dsc, main_row_dsc);
    lv_obj_set_grid_cell(name, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(temperature_val_t, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(humidity_val_t, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(btn1, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 2);

}

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
    }
}
