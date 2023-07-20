#include "lv_conf.h"
#include "board.h"
#include "lvgl.h"
#include "shell.h"
#include "ring_buffer.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "demos/lv_demos.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bl616_glb.h"
#include "bflb_dma.h"
#include "bflb_i2s.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "bflb_i2c.h"
#include "bflb_core.h"
#include "lv_font_smiley.c"

#include "tcp/tcp_client.h"

static lv_obj_t *obj0 = NULL;
static lv_obj_t *obj0_1 = NULL;

static lv_obj_t *er = NULL;
static lv_obj_t *san = NULL;
lv_obj_t *avatar_camera = NULL;
lv_obj_t *avatar_weather = NULL;

uint8_t get_photo_control = 0;

extern WeatherData *weather_data;

uint8_t isCameraGui = 0;
uint8_t isPlaceClothes = 0;

uint8_t quanshu = 0;
int wuti[4] = {0, 1, 2, 3};

static i = 0;
uint8_t new_clothe = -1;
int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;

static void clear_box() {
    // if(avatar_camera != NULL)
    //     lv_obj_del(avatar_camera);
    lv_obj_del(obj0_1);
    lv_obj_del(obj0);

    // if(er != NULL)
    //     lv_obj_del(er);

    // if(san != NULL)
    //     lv_obj_del(san);
}

void sendPWMStart() {
    if (quanshu > 0) {
        char string[16] = {0};
        itoa(quanshu, string, 10);
        strcat(string, "PWM");
        tcp_client_send(string, sizeof(string));
    }
}

static void my_box1(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch (code) {
        case LV_EVENT_LONG_PRESSED:


            if (flag1 == 0) {
                lv_label_set_text(label, "N0.1 yes");
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);

                quanshu = (4 - wuti[0]) % 4;
                printf("quanshu=%d\n", quanshu);
                // sendPWMStart();
                new_clothe = 1;
                for (i = 0; i <= 4; i++) {
                    wuti[i] += quanshu;
                    wuti[i] = wuti[i] % 4;
                }
                printf("wuti1=%d,wuti2=%d,wuti3=%d,wuti4=%d\n", wuti[0], wuti[1], wuti[2], wuti[3]);
                printf("box1\n");

                bflb_mtimer_delay_ms(50);
                gui_camera();
                isCameraGui = 1;
                isPlaceClothes = 1;
                flag1 = 1;
            }
            if (flag1 == 1) {
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);
                lv_label_set_text(label, "N0.1 no");
            }
            break;
        default:
            break;
    }
}


static void my_box2(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch (code) {
        case LV_EVENT_LONG_PRESSED:
            if (flag2 == 0) {
                lv_label_set_text(label, "NO.2 yes");
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);

                quanshu = (4 - wuti[1]) % 4;
                printf("quanshu=%d\n", quanshu);
                new_clothe = 2;
                for (i = 0; i <= 4; i++) {
                    wuti[i] += quanshu;
                    wuti[i] = wuti[i] % 4;
                }
                printf("wuti1=%d,wuti2=%d,wuti3=%d,wuti4=%d\n", wuti[0], wuti[1], wuti[2], wuti[3]);
                printf("box2\n");

                bflb_mtimer_delay_ms(50);
                gui_camera();
                isCameraGui = 1;
                isPlaceClothes = 1;
                flag2 = 1;
            }
            if (flag2 == 1) {
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);
                lv_label_set_text(label, " NO.2 no");
            }
            break;
        default:
            break;
    }
}


static void my_box3(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch (code) {
        case LV_EVENT_LONG_PRESSED:
            if (flag3 == 0) {
                lv_label_set_text(label, " NO.3 yes");
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);

                quanshu = (4 - wuti[2]) % 4;
                printf("quanshu=%d\n", quanshu);
                new_clothe = 3;
                for (i = 0; i <= 4; i++) {
                    wuti[i] += quanshu;
                    wuti[i] = wuti[i] % 4;
                }
                printf("wuti1=%d,wuti2=%d,wuti3=%d,wuti4=%d\n", wuti[0], wuti[1], wuti[2], wuti[3]);
                printf("box3\n");

                bflb_mtimer_delay_ms(50);
                gui_camera();
                isCameraGui = 1;
                isPlaceClothes = 1;
                flag3 = 1;
            }
            if (flag3 == 1) {
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);
                lv_label_set_text(label, " NO.3 no");
            }
            break;
        default:
            break;
    }
}


static void my_box4(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch (code) {
        case LV_EVENT_LONG_PRESSED:
            if (flag4 == 0) {
                lv_label_set_text(label, " NO.4 yes");
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);

                quanshu = (4 - wuti[3]) % 4;
                printf("quanshu=%d\n", quanshu);
                new_clothe = 4;
                for (i = 0; i <= 4; i++) {
                    wuti[i] += quanshu;
                    wuti[i] = wuti[i] % 4;
                }
                printf("wuti1=%d,wuti2=%d,wuti3=%d,wuti4=%d\n", wuti[0], wuti[1], wuti[2], wuti[3]);
                printf("box4\n");

                bflb_mtimer_delay_ms(50);
                gui_camera();
                isCameraGui = 1;
                isPlaceClothes = 1;
                flag4 = 1;
            }
            if (flag4 == 1) {
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);
                lv_label_set_text(label, " NO.4 no");
            }
            break;
        default:
            break;
    }
}


static void my_box1_1(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch (code) {
        case LV_EVENT_LONG_PRESSED:
            if (flag1 == 1) {
                lv_label_set_text(label, "N0.1 ");
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);

                quanshu = (4 - wuti[0]) % 4;
                printf("quanshu=%d\n", quanshu);
                sendPWMStart();
                for (i = 0; i <= 4; i++) {
                    wuti[i] += quanshu;
                    wuti[i] = wuti[i] % 4;
                }
                printf("wuti1=%d,wuti2=%d,wuti3=%d,wuti4=%d\n", wuti[0], wuti[1], wuti[2], wuti[3]);
                printf("box1\n");
                flag1 = 0;
            }
            if (flag1 == 0) {
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);
                lv_label_set_text(label, "N0.1 ");
            }

            break;
        default:

            break;
    }
}


static void my_box2_1(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch (code) {
        case LV_EVENT_LONG_PRESSED:
            if (flag2 == 1) {
                lv_label_set_text(label, "N0.2");
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);


                quanshu = (4 - wuti[1]) % 4;
                printf("quanshu=%d\n", quanshu);
                sendPWMStart();
                for (i = 0; i <= 4; i++) {
                    wuti[i] += quanshu;
                    wuti[i] = wuti[i] % 4;
                }
                printf("wuti1=%d,wuti2=%d,wuti3=%d,wuti4=%d\n", wuti[0], wuti[1], wuti[2], wuti[3]);
                printf("box2\n");
                flag2 = 0;
            }
            if (flag2 == 0) {
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);
                lv_label_set_text(label, "N0.2 ");
            }
            break;
        default:

            break;
    }
}


static void my_box3_1(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch (code) {
        case LV_EVENT_LONG_PRESSED:
            if (flag3 == 1) {
                lv_label_set_text(label, "N0.3 ");
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);


                quanshu = (4 - wuti[2]) % 4;
                printf("quanshu=%d\n", quanshu);
                sendPWMStart();
                for (i = 0; i <= 4; i++) {
                    wuti[i] += quanshu;
                    wuti[i] = wuti[i] % 4;
                }
                printf("wuti1=%d,wuti2=%d,wuti3=%d,wuti4=%d\n", wuti[0], wuti[1], wuti[2], wuti[3]);
                printf("box3\n");

                flag3 = 0;
            }
            if (flag3 == 0) {
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);
                lv_label_set_text(label, "N0.3 ");
            }
            break;
        default:

            break;
    }
}


static void my_box4_1(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    switch (code) {
        case LV_EVENT_LONG_PRESSED:
            lv_label_set_text(label, "N0.4 ");
            if (flag4 == 1) {
                lv_label_set_text(label, "N0.4 ");
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);

                quanshu = (4 - wuti[3]) % 4;
                printf("quanshu=%d\n", quanshu);
                sendPWMStart();
                for (i = 0; i <= 4; i++) {
                    wuti[i] += quanshu;
                    wuti[i] = wuti[i] % 4;
                }
                printf("wuti1=%d,wuti2=%d,wuti3=%d,wuti4=%d\n", wuti[0], wuti[1], wuti[2], wuti[3]);
                printf("box4\n");
                flag4 = 0;
            }
            if (flag4 == 0) {
                lv_obj_set_style_bg_color(obj, lv_color_hex(0xa3deb6), 0);
                lv_label_set_text(label, "N0.4 ");
            }
            break;
        default:

            break;
    }
}


static void button_event_place_clothes(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
        case LV_EVENT_SHORT_CLICKED:
            new_clothe = -1;
            lv_obj_set_style_bg_color(obj0_1, lv_color_hex(0xa3deb6), 0);
            gui_place_clothes();
            clear_box();
            break;
        default:

            break;
    }
}


static void button_event_takeout_clothes(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
        case LV_EVENT_SHORT_CLICKED:
            lv_obj_set_style_bg_color(obj0_1, lv_color_hex(0xa3deb6), 0);
            gui_takeout_clothes();
            clear_box();
            break;
        default:

            break;
    }
}

static void button_event_back_2_menu(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *sw = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED && lv_obj_get_state(sw) & LV_STATE_CHECKED) {
        // clear_box();
        gui_menu();
        isCameraGui = 0;
    }
}

static void button_event_camera(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_SHORT_CLICKED) {
        lv_obj_set_style_bg_color(obj0_1, lv_color_hex(0xa3deb6), 0);
        gui_camera();
        clear_box();
        isCameraGui = 1;
    }
}

static void button_event_weather(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_SHORT_CLICKED) {
        lv_obj_set_style_bg_color(obj0_1, lv_color_hex(0xa3deb6), 0);
        gui_weather();
        clear_box();
        isCameraGui = 0;
    }
}

static const uint8_t take_photo[] = "get_photo\r";

static void button_event_take_photo(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {
        printf("TAKING PHOTO!\r\n");
        get_photo_control = 1;
        // while (get_photo_control)
        // {
        //     bflb_mtimer_delay_ms(10);
        // }
        // if (isPlaceClothes)
        // {
        //     lv_obj_set_style_bg_color(obj0_1, lv_color_hex(0xa3deb6), 0);
        //     gui_place_clothes();
        //     // clear_box();
        //     isPlaceClothes = 0;
        // }
    }
}

// static const uint8_t title_city[] = "城市: ";
// static const uint8_t title_weather[] = "天气: ";
// static const uint8_t title_temperature[] = "气温: ";
// static const uint8_t title_windDirection[] = "风向: ";
// static const uint8_t title_windPower[] = "风力: ";
// static const uint8_t title_humidity[] = "湿度: ";

void gui_weather() {
    LV_FONT_DECLARE(lv_font_smiley);

    avatar_weather = lv_obj_create(lv_scr_act());
    lv_obj_set_size(avatar_weather, 240, 320);
    lv_obj_set_style_bg_color(avatar_weather, lv_color_hex(0xf6e9ed), 0);

    lv_obj_t *sw = lv_switch_create(avatar_weather);
    lv_obj_set_pos(sw, 10, 30);
    lv_obj_add_event_cb(sw, button_event_back_2_menu, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *label_city = lv_label_create(avatar_weather);
    lv_obj_align_to(label_city, avatar_weather, LV_ALIGN_TOP_LEFT, 20, 75);
    lv_label_set_text_fmt(label_city, "城市: %s", weather_data->province);
    lv_obj_set_style_text_font(label_city, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *label_weather = lv_label_create(avatar_weather);
    lv_obj_align_to(label_weather, avatar_weather, LV_ALIGN_TOP_LEFT, 20, 100);
    lv_label_set_text_fmt(label_weather, "天气: %s", weather_data->weather);
    lv_obj_set_style_text_font(label_weather, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_t *label_temperature = lv_label_create(avatar_weather);
    lv_obj_align_to(label_temperature, avatar_weather, LV_ALIGN_TOP_LEFT, 20, 125);
    lv_label_set_text_fmt(label_temperature, "气温: %d", weather_data->temperature);
    lv_obj_set_style_text_font(label_temperature, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *label_windDirection = lv_label_create(avatar_weather);
    lv_obj_align_to(label_windDirection, avatar_weather, LV_ALIGN_TOP_LEFT, 20, 150);
    lv_label_set_text_fmt(label_windDirection, "风向: %s", weather_data->windDirection);
    lv_obj_set_style_text_font(label_windDirection, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *label_windPower = lv_label_create(avatar_weather);
    lv_obj_align_to(label_windPower, avatar_weather, LV_ALIGN_TOP_LEFT, 20, 175);
    lv_label_set_text_fmt(label_windPower, "风力: %s 级", weather_data->windPower);
    lv_obj_set_style_text_font(label_windPower, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *label_humidity = lv_label_create(avatar_weather);
    lv_obj_align_to(label_humidity, avatar_weather, LV_ALIGN_TOP_LEFT, 20, 200);
    lv_label_set_text_fmt(label_humidity, "湿度: %d", weather_data->humidity);
    lv_obj_set_style_text_font(label_humidity, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void gui_camera() {
    LV_FONT_DECLARE(lv_font_smiley);
    avatar_camera = lv_img_create(lv_scr_act());
    lv_obj_t *sw = lv_switch_create(avatar_camera);
    lv_obj_set_pos(sw, 10, 30);
    lv_obj_add_event_cb(sw, button_event_back_2_menu, LV_EVENT_VALUE_CHANGED, NULL);

    lv_img_set_angle(avatar_camera, /**1800*/0);
    lv_obj_align(avatar_camera, LV_ALIGN_CENTER, -1, -1);

    lv_obj_t *obj1 = lv_obj_create(avatar_camera);
    lv_obj_set_size(obj1, 240, 30);
    lv_obj_align_to(obj1, avatar_camera, LV_ALIGN_BOTTOM_MID, 0, 260);

    lv_obj_t *label1 = lv_label_create(obj1);
    lv_obj_align_to(label1, obj1, LV_ALIGN_CENTER, -2, 0);
    lv_label_set_text(label1, "拍摄");
    lv_obj_set_style_text_font(label1, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(obj1, button_event_take_photo, LV_EVENT_ALL, NULL);
}

void gui_menu(void)  //菜单页
{
    LV_FONT_DECLARE(lv_font_smiley);
    obj0 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj0, 240, 99);
    lv_obj_set_pos(obj0, 0, 0);
    lv_obj_set_style_bg_color(obj0, lv_color_hex(0xe0f2e1), 0);

    obj0_1 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj0_1, 240, 220);
    lv_obj_set_pos(obj0_1, 0, 100);
    lv_obj_set_style_bg_color(obj0_1, lv_color_hex(0xe0f2e1), 0);

    lv_obj_t *label = lv_label_create(obj0);
    lv_obj_align_to(label, obj0, LV_ALIGN_TOP_MID, -24, 20);
    lv_label_set_text(label, "智能衣柜");
    lv_obj_set_style_text_font(label, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *obj1 = lv_obj_create(obj0_1);
    lv_obj_set_size(obj1, 100, 80);
    lv_obj_align_to(obj1, obj0_1, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *obj2 = lv_obj_create(obj0_1);
    lv_obj_set_size(obj2, 100, 80);
    lv_obj_align_to(obj2, obj0_1, LV_ALIGN_TOP_RIGHT, 0, 0);

    //  lv_obj_t *obj3=lv_obj_create(obj0_1);
    //  lv_obj_set_size(obj3,100,80);
    //  lv_obj_align_to(obj3,obj1,LV_ALIGN_OUT_BOTTOM_MID,0,10);

    lv_obj_t *obj4 = lv_obj_create(obj0_1);
    lv_obj_set_size(obj4, 100, 80);
    lv_obj_align_to(obj4, obj1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_t *obj5 = lv_obj_create(obj0_1);
    lv_obj_set_size(obj5, 100, 80);
    lv_obj_align_to(obj5, obj2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // lv_obj_t *obj6 = lv_obj_create(obj0_1);
    // lv_obj_set_size(obj6, 100, 80);
    // lv_obj_align_to(obj6, obj4, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);


    lv_obj_t *label1 = lv_label_create(obj1);
    lv_obj_align_to(label1, obj1, LV_ALIGN_CENTER, -20, 0);
    lv_label_set_text(label1, "放置衣物");
    lv_obj_set_style_text_font(label1, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *label2 = lv_label_create(obj2);
    lv_obj_align_to(label2, obj2, LV_ALIGN_CENTER, -20, 0);
    lv_label_set_text(label2, "拿取衣物");
    lv_obj_set_style_text_font(label2, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    //  lv_obj_t *label3=lv_label_create(obj3); 
    //  lv_obj_align_to(label3,obj3,LV_ALIGN_CENTER,0,0);
    //  lv_label_set_text(label3,"wifi");

    lv_obj_t *label4 = lv_label_create(obj4);
    lv_obj_align_to(label4, obj4, LV_ALIGN_LEFT_MID, 12, 0);
    lv_label_set_text(label4, "摄像头");
    lv_obj_set_style_text_font(label4, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *label5 = lv_label_create(obj5);
    lv_obj_align_to(label5, obj5, LV_ALIGN_CENTER, -2, 0);
    lv_label_set_text(label5, "天气");
    lv_obj_set_style_text_font(label5, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    //lv_obj_t *label6 = lv_label_create(obj6); 
    //lv_obj_align_to(label6, obj6, LV_ALIGN_LEFT_MID, 10, 0);
    //lv_label_set_text(label6, "Record");



    lv_obj_add_event_cb(obj1, button_event_place_clothes, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(obj2, button_event_takeout_clothes, LV_EVENT_ALL, NULL);
    //lv_obj_add_event_cb(obj3,box3,LV_EVENT_ALL,NULL);
    lv_obj_add_event_cb(obj4, button_event_camera, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(obj5, button_event_weather, LV_EVENT_ALL, NULL);
    //lv_obj_add_event_cb(obj6,box6,LV_EVENT_ALL,NULL);
}


void gui_place_clothes(void) {
    new_clothe = -1;
    LV_FONT_DECLARE(lv_font_smiley);
    er = lv_obj_create(lv_scr_act());
    lv_obj_set_size(er, 240, 320);
    lv_obj_t *labe1_1 = lv_label_create(er);
    lv_obj_set_style_bg_color(er, lv_color_hex(0xf6e9ed), 0);

    lv_obj_t *sw = lv_switch_create(er);
    lv_obj_set_pos(sw, 0, 8);
    lv_obj_add_event_cb(sw, button_event_back_2_menu, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *labe1_2 = lv_label_create(er);
    lv_obj_set_pos(labe1_2, 120, 0);
    lv_label_set_text(labe1_2, " 绿色:已放置\n 白色:未放置");
    lv_obj_set_style_text_font(labe1_2, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_label_set_text(labe1_1, "hello");


    lv_obj_t *obj1_1 = lv_obj_create(er);
    lv_obj_t *obj1_2 = lv_obj_create(er);
    lv_obj_t *obj1_3 = lv_obj_create(er);
    lv_obj_t *obj1_4 = lv_obj_create(er);

    lv_obj_set_size(obj1_1, 80, 80);
    lv_obj_set_size(obj1_2, 80, 80);
    lv_obj_set_size(obj1_3, 80, 80);
    lv_obj_set_size(obj1_4, 80, 80);

    lv_obj_set_pos(labe1_1, 80, 55);
    lv_obj_set_pos(obj1_1, 10, 80);
    lv_obj_set_pos(obj1_2, 130, 80);
    lv_obj_set_pos(obj1_3, 10, 200);
    lv_obj_set_pos(obj1_4, 130, 200);

    lv_obj_add_event_cb(obj1_1, my_box1, LV_EVENT_ALL, labe1_1);
    lv_obj_add_event_cb(obj1_2, my_box2, LV_EVENT_ALL, labe1_1);
    lv_obj_add_event_cb(obj1_3, my_box3, LV_EVENT_ALL, labe1_1);
    lv_obj_add_event_cb(obj1_4, my_box4, LV_EVENT_ALL, labe1_1);

    if (flag1 == 1) {
        lv_obj_set_style_bg_color(obj1_1, lv_color_hex(0xa3deb6), 0);
    }
    if (flag2 == 1) {
        lv_obj_set_style_bg_color(obj1_2, lv_color_hex(0xa3deb6), 0);
    }
    if (flag3 == 1) {
        lv_obj_set_style_bg_color(obj1_3, lv_color_hex(0xa3deb6), 0);
    }
    if (flag4 == 1) {
        lv_obj_set_style_bg_color(obj1_4, lv_color_hex(0xa3deb6), 0);
    }

}

void gui_takeout_clothes(void) {
    LV_FONT_DECLARE(lv_font_smiley);
    san = lv_obj_create(lv_scr_act());
    lv_obj_set_size(san, 240, 320);
    lv_obj_t *labe3_1 = lv_label_create(san);
    lv_obj_set_style_bg_color(san, lv_color_hex(0xf6e9ed), 0);

    lv_obj_t *sw = lv_switch_create(san);
    lv_obj_set_pos(sw, 0, 8);
    lv_obj_add_event_cb(sw, button_event_back_2_menu, LV_EVENT_VALUE_CHANGED, NULL);//LV_EVENT_VALUE_CHANGED 开关触发时事件状态

    lv_obj_t *labe3_2 = lv_label_create(san);
    lv_obj_set_pos(labe3_2, 120, 0);
    lv_label_set_text(labe3_2, " 绿色:已取出\n 白色:未取出");
    lv_obj_set_style_text_font(labe3_2, &lv_font_smiley, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_label_set_text(labe3_1, "hello");

    lv_obj_t *obj3_1 = lv_obj_create(san);
    lv_obj_t *obj3_2 = lv_obj_create(san);
    lv_obj_t *obj3_3 = lv_obj_create(san);
    lv_obj_t *obj3_4 = lv_obj_create(san);

    lv_obj_set_size(obj3_1, 80, 80);
    lv_obj_set_size(obj3_2, 80, 80);
    lv_obj_set_size(obj3_3, 80, 80);
    lv_obj_set_size(obj3_4, 80, 80);

    lv_obj_set_pos(labe3_1, 90, 55);
    lv_obj_set_pos(obj3_1, 10, 80);
    lv_obj_set_pos(obj3_2, 130, 80);
    lv_obj_set_pos(obj3_3, 10, 200);
    lv_obj_set_pos(obj3_4, 130, 200);

    lv_obj_add_event_cb(obj3_1, my_box1_1, LV_EVENT_ALL, labe3_1);
    lv_obj_add_event_cb(obj3_2, my_box2_1, LV_EVENT_ALL, labe3_1);
    lv_obj_add_event_cb(obj3_3, my_box3_1, LV_EVENT_ALL, labe3_1);
    lv_obj_add_event_cb(obj3_4, my_box4_1, LV_EVENT_ALL, labe3_1);

    if (flag1 == 0) {
        lv_obj_set_style_bg_color(obj3_1, lv_color_hex(0xa3deb6), 0);
    }
    if (flag2 == 0) {
        lv_obj_set_style_bg_color(obj3_2, lv_color_hex(0xa3deb6), 0);
    }
    if (flag3 == 0) {
        lv_obj_set_style_bg_color(obj3_3, lv_color_hex(0xa3deb6), 0);
    }
    if (flag4 == 0) {
        lv_obj_set_style_bg_color(obj3_4, lv_color_hex(0xa3deb6), 0);
    }

}
