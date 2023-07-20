#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "mem.h"
#include "stack_macros.h"
#include "board.h"
#include "shell.h"
#include "ring_buffer.h"
#include "log.h"


#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "tcp/tcp_client.h"

#include "bl_fw_api.h"
#include "rfparam_adapter.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bl616_glb.h"
#include "bflb_dma.h"
#include "bflb_i2s.h"
#include "bflb_clock.h"
#include "bflb_i2c.h"
#include "bflb_cam.h"

#include "lv_conf.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "demos/lv_demos.h"
#include "lvgl_control.c"

#include "lcd.h"

#include "image_sensor.h"

#define DBG_TAG "MAIN"


//WIFI参数
#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

//摄像头
#define CROP_WQVGA_X        (240)
#define CROP_WQVGA_Y        (320)
#define CAM_BUFF_NUM        (4)


uint8_t *pic;
static uint8_t picture[CROP_WQVGA_X * CROP_WQVGA_Y * CAM_BUFF_NUM] ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(64)));

static struct bflb_device_s *uart0;

//任务句柄
static TaskHandle_t wifi_fw_task;
static TaskHandle_t wifi_task_handle;
static TaskHandle_t lv_camera_task_handle;
static TaskHandle_t send_photo_task_handle;

//SHELL指令
static const uint8_t wifi_sta_connet[] = "wifi_sta_connect ssid psw\r";
static const uint8_t connet_server[] = "connect_server\r";

extern Ring_Buffer_Type shell_rb;
extern void shell_release_sem(void);

//任务状态
SemaphoreHandle_t sem_wifi_init_done;
SemaphoreHandle_t sem_wifi_connect_done;
SemaphoreHandle_t sem_wifi_disconnect;


static struct bflb_device_s *i2c0;
static struct bflb_device_s *cam0;
struct bflb_cam_config_s cam_config;
struct image_sensor_config_s *sensor_config;

uint8_t finish_get_photo[6] = {'P','H','_','E','N','D'};

extern lv_obj_t *avatar_camera;
extern uint8_t isCameraGui;
extern uint8_t isPlaceClothes;
extern uint8_t get_photo_control;
extern uint8_t quanshu;
extern uint8_t new_clothe;


static wifi_conf_t conf = {
    .country_code = "CN",
};

lv_img_dsc_t img_src_cam = {
        .header.always_zero = 0,
        .header.w = CROP_WQVGA_X,
        .header.h = CROP_WQVGA_Y,
        .data_size = CROP_WQVGA_X * CROP_WQVGA_Y * (CAM_BUFF_NUM / 2),
        .header.cf = LV_IMG_CF_TRUE_COLOR,
    };
