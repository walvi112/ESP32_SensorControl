#include "sensorControl.h"
#include "img_background.h"

#define RECOM_WIDTH     320
#define RECOM_HEIGHT    240

static void window_delete_event_cb(lv_event_t *e);
static void imu_create(lv_obj_t *parent);
static void temperature_create(lv_obj_t *parent);
static void remove_padding(lv_obj_t *obj);

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;



static disp_size_t disp_size;
static const lv_font_t *font_large;
static const lv_font_t *font_normal;

static lv_style_t style_cont;
static lv_style_t style_sensor_cont;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;



void sensorControl(void)
{
    if (((float) LV_HOR_RES / LV_VER_RES) != ((float) 320 / 240))
        LV_LOG_WARN("This aspect ratio is not recommended for best display.");

    if (LV_HOR_RES <= 320)
        disp_size = DISP_SMALL;
    else if(LV_HOR_RES < 720)
        disp_size = DISP_MEDIUM;
    else
        disp_size = DISP_LARGE;

    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

    if(disp_size == DISP_LARGE)
    {
        #if LV_FONT_MONTSERRAT_24
            font_large = &lv_font_montserrat_24;
        #else
            LV_LOG_WARN("LV_FONT_MONTSERRAT_24 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
        #endif
        #if LV_FONT_MONTSERRAT_16
            font_normal = &lv_font_montserrat_16;
        #else
            LV_LOG_WARN("LV_FONT_MONTSERRAT_16 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
        #endif
    }
    else if(disp_size == DISP_MEDIUM)
    {
        #if LV_FONT_MONTSERRAT_20
            font_large = &lv_font_montserrat_20;
        #else
            LV_LOG_WARN("LV_FONT_MONTSERRAT_20 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
        #endif
        #if LV_FONT_MONTSERRAT_14
            font_normal = &lv_font_montserrat_14;
        #else
            LV_LOG_WARN("LV_FONT_MONTSERRAT_14 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
        #endif
    }
    else
    {   /* disp_size == DISP_SMALL */
        #if LV_FONT_MONTSERRAT_18
            font_large = &lv_font_montserrat_18;
        #else
            LV_LOG_WARN("LV_FONT_MONTSERRAT_18 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
        #endif
        #if LV_FONT_MONTSERRAT_12
            font_normal = &lv_font_montserrat_12;
        #else
            LV_LOG_WARN("LV_FONT_MONTSERRAT_12 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
        #endif
    }

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

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);


    LV_IMAGE_DECLARE(img_background);
    lv_obj_t * background = lv_image_create(lv_screen_active());
    lv_image_set_src(background, &img_background);
    lv_image_set_inner_align(background, LV_IMAGE_ALIGN_CENTER);
    lv_obj_set_size(background, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(background);

    lv_obj_set_layout(background, LV_LAYOUT_GRID);
    static int32_t main_column_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t main_row_dsc[] = {
            LV_GRID_CONTENT,  /*Date*/
            LV_GRID_CONTENT,  /*Time*/
            10,
            LV_GRID_FR(1),  /*Sensor*/
            LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(background, main_column_dsc, main_row_dsc);


    lv_obj_t *date = lv_label_create(background);
    lv_label_set_text(date, "Monday 7th");
    lv_obj_add_style(date, &style_title, 0);
    lv_obj_set_style_pad_top(date, 10 , 0);


    lv_obj_t *time = lv_label_create(background);
    lv_label_set_text(time, "00:00");
    lv_obj_add_style(time, &style_title, 0);

    lv_obj_t *sensor_cont = lv_obj_create(background);
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
        lv_style_reset(&style_title);
        lv_style_reset(&style_icon);
        lv_style_reset(&style_bullet);
    }
}
