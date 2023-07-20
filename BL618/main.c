#include "header.h"


extern void shell_init_with_task(struct bflb_device_s *shell);

//WIFI自动初始化
int wifi_start_firmware_task(void) {
    LOG_I("Starting wifi ...\r\n");

    /* enable wifi clock */

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* set ble controller EM Size */

    GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    /* Enable wifi irq */

    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback) interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    xTaskCreate(wifi_main, (char *) "fw", WIFI_STACK_SIZE, NULL, TASK_PRIORITY_FW, &wifi_fw_task);

    return 0;
}

void wifi_event_handler(uint32_t code) {
    uint32_t ret = 0;
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);
        }
            break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
            ret = xSemaphoreGiveFromISR(sem_wifi_init_done, &xHigherPriorityTaskWoken);
            if (ret == pdPASS) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        }
            break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        }
            break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        }
            break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size());
            ret = xSemaphoreGiveFromISR(sem_wifi_connect_done, &xHigherPriorityTaskWoken);
            if (ret == pdPASS) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        }
            break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
            ret = xSemaphoreGiveFromISR(sem_wifi_disconnect, &xHigherPriorityTaskWoken);
            if (ret == pdPASS) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        }
            break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        }
            break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        }
            break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        }
            break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        }
            break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

void lv_log_print_g_cb(const char *buf) {
    printf("[LVGL] %s", buf);
}

void init_camera() {
    board_dvp_gpio_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    cam0 = bflb_device_get_by_name("cam0");

    if (image_sensor_scan(i2c0, &sensor_config)) {
        LOG_I("\r\nSensor name: %s\r\n", sensor_config->name);
    } else {
        LOG_E("\r\nError! Can't identify sensor!\r\n");
        while (1) {
        }
    }
    /* Crop resolution_x, should be set before init */
    bflb_cam_crop_hsync(cam0, 112, 112 + CROP_WQVGA_X);
    /* Crop resolution_y, should be set before init */
    bflb_cam_crop_vsync(cam0, 120, 120 + CROP_WQVGA_Y);

    memcpy(&cam_config, sensor_config, IMAGE_SENSOR_INFO_COPY_SIZE);
    cam_config.with_mjpeg = false;
    cam_config.input_source = CAM_INPUT_SOURCE_DVP;
    cam_config.output_format = CAM_OUTPUT_FORMAT_RGB565_OR_BGR565;
    cam_config.output_bufaddr = (uint32_t) picture;
    cam_config.output_bufsize = CROP_WQVGA_X * CROP_WQVGA_Y * (CAM_BUFF_NUM / 2);

    bflb_cam_init(cam0, &cam_config);

}

void wifi_task(void *param) {
    if (xSemaphoreTake(sem_wifi_init_done, portMAX_DELAY) == pdTRUE) {
        LOG_I("Start connect wifi..");
        // vTaskDelay(5000);
        Ring_Buffer_Write(&shell_rb, wifi_sta_connet, sizeof(wifi_sta_connet) - 1);
        shell_release_sem();
    }

    if (xSemaphoreTake(sem_wifi_connect_done, portMAX_DELAY) == pdTRUE) {
        Ring_Buffer_Write(&shell_rb, connet_server, sizeof(connet_server) - 1);
        shell_release_sem();
        xTaskCreate(lv_task, "lv_task", 10240, NULL, 0, &lv_camera_task_handle);
        // xTaskCreate(lv_camera_task, "lv_camera_task", 10240, NULL, 0, &lv_camera_task_handle);
    }

    while (1) {
        if (xSemaphoreTake(sem_wifi_disconnect, portMAX_DELAY) == pdTRUE) {
            Ring_Buffer_Write(&shell_rb, wifi_sta_connet, sizeof(wifi_sta_connet) - 1);
            shell_release_sem();
        }
        vTaskDelay(10000);
    }
}


void lv_task(void *param) {
    uint32_t pic_size;
    uint32_t tcp_data_len = CROP_WQVGA_X * sizeof(uint8_t);
    gui_menu();
    while (1) {
        lv_task_handler();
        bflb_mtimer_delay_ms(1);

        if (isCameraGui) {
            bflb_cam_start(cam0);
            if (bflb_cam_get_frame_count(cam0) > 0) {
                pic_size = bflb_cam_get_frame_info(cam0, &pic);
                bflb_cam_pop_one_frame(cam0);
                if (get_photo_control == 1) {
                    bflb_cam_stop(cam0);
                    for (i = 0; i < CROP_WQVGA_Y * 2; i++) {
                        tcp_client_send(pic + (i * CROP_WQVGA_X), tcp_data_len);
                        // while(tcp_write_check(tcp_data_len) != ERR_OK){}
                        bflb_mtimer_delay_ms(15);
                    }
                    bflb_mtimer_delay_ms(20);
                    tcp_client_write(finish_get_photo, sizeof(finish_get_photo));

                    get_photo_control = 0;
                    if (!isPlaceClothes) {
                        bflb_cam_start(cam0);
                        tcp_client_output();
                    } else {
                        bflb_cam_stop(cam0);
                        char numberClothe[2] = {0};
                        itoa(new_clothe, numberClothe, 10);
                        tcp_client_send(numberClothe, sizeof(numberClothe));
                        if (quanshu > 0) {
                            char string[16] = {0};
                            itoa(quanshu, string, 10);
                            strcat(string, "PWM");
                            tcp_client_send(string, sizeof(string));
                        }
                        gui_place_clothes();
                    }
                    bflb_cam_start(cam0);
                }

                img_src_cam.data = pic;
                lv_img_set_src(avatar_camera, &img_src_cam);
            }
        } else {
            bflb_cam_stop(cam0);
        }

    }
}


int main(void) {
    board_init();

    lv_log_register_print_cb(lv_log_print_g_cb);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    init_camera();

    sem_wifi_init_done = xSemaphoreCreateBinary();
    sem_wifi_connect_done = xSemaphoreCreateBinary();
    sem_wifi_disconnect = xSemaphoreCreateBinary();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    tcpip_init(NULL, NULL);
    wifi_start_firmware_task();

    xTaskCreate(wifi_task, "wifi_task", 2048, NULL, 15, &wifi_task_handle);

    vTaskStartScheduler();

    while (1) {
    }
}