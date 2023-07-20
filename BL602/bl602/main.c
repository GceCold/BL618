#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>

#include <lwip/tcpip.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/err.h>
#include "lwip/api.h"
#include "lwip/arch.h"
#include "lwip/opt.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include <http_client.h>
#include <netutils/netutils.h>

#include <bl602_glb.h>
#include <bl602_hbn.h>

#include <bl_uart.h>
#include <bl_chip.h>
#include <bl_wifi.h>
#include <hal_wifi.h>
#include <bl_sec.h>
#include <bl_cks.h>
#include <bl_irq.h>
#include <bl_dma.h>
#include <bl_timer.h>
#include "bl_gpio.h"
#include <bl_gpio_cli.h>
#include <bl_wdt_cli.h>
#include <bl_pwm.h>
#include <hal_uart.h>
#include <hal_sys.h>
#include <hal_gpio.h>
#include <hal_boot2.h>
#include <hal_board.h>
#include <hal_pwm.h>
#include <looprt.h>
#include <loopset.h>
#include <sntp.h>
#include <bl_sys_time.h>
#include <bl_sys.h>
#include <bl_sys_ota.h>
#include <bl_romfs.h>
#include <fdt.h>

#include <easyflash.h>
#include <bl60x_fw_api.h>
#include <wifi_mgmr_ext.h>
#include <utils_log.h>
#include <libfdt.h>
#include <hal/soc/pwm.h>
#include <device/vfs_pwm.h>
#include <blog.h>


#define mainHELLO_TASK_PRIORITY     ( 20 )
#define UART_ID_2 (2)

#define TCP_CLIENT_TEST_PORT (7361)

#define SERVER_IP_ADDR0 (192)
#define SERVER_IP_ADDR1 (168)
#define SERVER_IP_ADDR2 (3)
#define SERVER_IP_ADDR3 (114)

#define RECV_DATA_TYPE_WEATHER (1)

static const uint8_t *BOARD_UUID = "UUID:dec74884-b6fb-42fc-980d-b4c5a6be0cee";

static const uint8_t send_buf[3] = {
        'p', 'w', 'm'
};

static struct tcp_pcb *client_pcb = NULL;
static StackType_t tcp_client_proc_stack[4096];
static StaticTask_t tcp_client_proc_task;

uint8_t DHT11_ReadTemHum(uint8_t *buf);

void DHT11_Start(void);

typedef struct {
    uint8_t province[128];
    uint8_t weather[64];
    uint8_t temperature;
    uint8_t windDirection[64];
    uint8_t windPower[64];
    uint8_t humidity;
} WeatherData;

WeatherData *weather_data;

extern void ble_stack_start(void);

/* TODO: const */
volatile uint32_t uxTopUsedPriority __attribute__((used)) = configMAX_PRIORITIES - 1;
static uint32_t time_main;

static wifi_conf_t conf =
        {
                .country_code = "CN",
        };
extern uint8_t _heap_start;
extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
extern uint8_t _heap_wifi_start;
extern uint8_t _heap_wifi_size; // @suppress("Type cannot be resolved")
static HeapRegion_t xHeapRegions[] =
        {
                {&_heap_start,      (unsigned int) &_heap_size}, //set on runtime
                {&_heap_wifi_start, (unsigned int) &_heap_wifi_size},
                {NULL,              0}, /* Terminates the array. */
                {NULL,              0} /* Terminates the array. */
        };
static wifi_interface_t wifi_interface;

char tcp_client_send(void *arg, uint32_t pic_len);

void tcp_client_raw_init(void);

char tcp_client_write(void *arg, uint32_t pic_len);

char tcp_client_output();

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    puts("Stack Overflow checked\r\n");
    printf("Task Name %s\r\n", pcTaskName);
    while (1) {
        /*empty here*/
    }
}

void vApplicationMallocFailedHook(void) {
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n",
           xPortGetFreeHeapSize()
    );
    while (1) {
        /*empty here*/
    }
}


static void wifi_sta_connect(char *ssid, char *password) {
    wifi_interface_t wifi_interface;

    wifi_interface = wifi_mgmr_sta_enable();
    wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
}

static void event_cb_wifi_event(input_event_t *event, void *private_data) {
    static char *ssid;
    static char *password;

    switch (event->code) {
        case CODE_WIFI_ON_INIT_DONE: {
            printf("[APP] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            wifi_mgmr_start_background(&conf);
        }
            break;
        case CODE_WIFI_ON_MGMR_DONE: {
            printf("[APP] [EVT] MGMR DONE %lld, now %lums\r\n", aos_now_ms(), bl_timer_now_us() / 1000);
            wifi_sta_connect("ssid", "psw");
            // _connect_wifi();
        }
            break;
        case CODE_WIFI_ON_MGMR_DENOISE: {
            printf("[APP] [EVT] Microwave Denoise is ON %lld\r\n", aos_now_ms());
        }
            break;
        case CODE_WIFI_ON_SCAN_DONE: {
            printf("[APP] [EVT] SCAN Done %lld\r\n", aos_now_ms());
            wifi_mgmr_cli_scanlist();
        }
            break;
        case CODE_WIFI_ON_SCAN_DONE_ONJOIN: {
            printf("[APP] [EVT] SCAN On Join %lld\r\n", aos_now_ms());
        }
            break;
        case CODE_WIFI_ON_DISCONNECT: {
            printf("[APP] [EVT] disconnect %lld, Reason: %s\r\n",
                   aos_now_ms(),
                   wifi_mgmr_status_code_str(event->value)
            );
        }
            break;
        case CODE_WIFI_ON_CONNECTING: {
            printf("[APP] [EVT] Connecting %lld\r\n", aos_now_ms());
        }
            break;
        case CODE_WIFI_CMD_RECONNECT: {
            printf("[APP] [EVT] Reconnect %lld\r\n", aos_now_ms());
        }
            break;
        case CODE_WIFI_ON_CONNECTED: {
            printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
        }
            break;
        case CODE_WIFI_ON_PRE_GOT_IP: {
            printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
        }
            break;
        case CODE_WIFI_ON_GOT_IP: {
            printf("[APP] [EVT] GOT IP %lld\r\n", aos_now_ms());
            printf("[SYS] Memory left is %d Bytes\r\n", xPortGetFreeHeapSize());
            tcp_client_raw_init();
        }
            break;
        case CODE_WIFI_ON_EMERGENCY_MAC: {
            printf("[APP] [EVT] EMERGENCY MAC %lld\r\n", aos_now_ms());
            hal_reboot();//one way of handling emergency is reboot. Maybe we should also consider solutions
        }
            break;
        case CODE_WIFI_ON_PROV_SSID: {
            printf("[APP] [EVT] [PROV] [SSID] %lld: %s\r\n",
                   aos_now_ms(),
                   event->value ? (const char *) event->value : "UNKNOWN"
            );
            if (ssid) {
                vPortFree(ssid);
                ssid = NULL;
            }
            ssid = (char *) event->value;
        }
            break;
        case CODE_WIFI_ON_PROV_BSSID: {
            printf("[APP] [EVT] [PROV] [BSSID] %lld: %s\r\n",
                   aos_now_ms(),
                   event->value ? (const char *) event->value : "UNKNOWN"
            );
            if (event->value) {
                vPortFree((void *) event->value);
            }
        }
            break;
        case CODE_WIFI_ON_PROV_PASSWD: {
            printf("[APP] [EVT] [PROV] [PASSWD] %lld: %s\r\n", aos_now_ms(),
                   event->value ? (const char *) event->value : "UNKNOWN"
            );
            if (password) {
                vPortFree(password);
                password = NULL;
            }
            password = (char *) event->value;
        }
            break;
        case CODE_WIFI_ON_PROV_CONNECT: {
            printf("[APP] [EVT] [PROV] [CONNECT] %lld\r\n", aos_now_ms());
            printf("connecting to %s:%s...\r\n", ssid, password);
            wifi_sta_connect(ssid, password);
        }
            break;
        case CODE_WIFI_ON_PROV_DISCONNECT: {
            printf("[APP] [EVT] [PROV] [DISCONNECT] %lld\r\n", aos_now_ms());
        }
            break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            printf("[APP] [EVT] [AP] [ADD] %lld, sta idx is %lu\r\n", aos_now_ms(), (uint32_t) event->value);
        }
            break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            printf("[APP] [EVT] [AP] [DEL] %lld, sta idx is %lu\r\n", aos_now_ms(), (uint32_t) event->value);
        }
            break;
        default: {
            printf("[APP] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
            /*nothing*/
        }
    }
}

void vApplicationIdleHook(void) {
#if 0
    __asm volatile(
            "   wfi     "
    );
    /*empty*/
#else
    (void) uxTopUsedPriority;
#endif
}

static void cmd_stack_wifi(char *buf, int len, int argc, char **argv) {
    /*wifi fw stack and thread stuff*/
    static uint8_t stack_wifi_init = 0;


    if (1 == stack_wifi_init) {
        puts("Wi-Fi Stack Started already!!!\r\n");
        return;
    }
    stack_wifi_init = 1;

    printf("Start Wi-Fi fw @%lums\r\n", bl_timer_now_us() / 1000);
    hal_wifi_start_firmware_task();
    /*Trigger to start Wi-Fi*/
    printf("Start Wi-Fi fw is Done @%lums\r\n", bl_timer_now_us() / 1000);
    aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);

}

static void cmd_send_data(char *buf, int len, int argc, char **argv) {
    tcp_write(client_pcb, send_buf, sizeof(send_buf), NETCONN_COPY);
    tcp_output(client_pcb);
    printf("Send Data...\r\n");
}

uint8_t DHT11Data[4] = {0};
uint8_t Temp, Humi;

static void cmd_get_temp(char *buf, int len, int argc, char **argv) {
    DHT11_Start();
    if (DHT11_ReadTemHum(DHT11Data)) {
        Temp = DHT11Data[2];
        Humi = DHT11Data[0];

        printf("Temp=%d, Humi=%d\r\n", Temp, Humi);
    } else {
        printf("DHT11 Error!\r\n");
    }
}


const static struct cli_command cmds_user[]
STATIC_CLI_CMD_ATTRIBUTE = {
        /*Stack Command*/
        {"stack_wifi", "Wi-Fi Stack", cmd_stack_wifi},
        {"send_data",  "get weather", cmd_send_data},
        {"get_temp",   "get weather", cmd_get_temp},
};

static void _cli_init() {
    /*Put CLI which needs to be init here*/
    int codex_debug_cli_init(void);
    codex_debug_cli_init();
    easyflash_cli_init();
    network_netutils_iperf_cli_register();
    network_netutils_tcpserver_cli_register();
    network_netutils_tcpclinet_cli_register();
    network_netutils_netstat_cli_register();
    network_netutils_ping_cli_register();
    sntp_cli_init();
    bl_sys_time_cli_init();
    bl_sys_ota_cli_init();
    blfdt_cli_init();
    wifi_mgmr_cli_init();
    bl_wdt_cli_init();
    bl_gpio_cli_init();
    looprt_test_cli_init();
}


static int get_dts_addr(const char *name, uint32_t *start, uint32_t *off) {
    uint32_t addr = hal_board_get_factory_addr();
    const void *fdt = (const void *) addr;
    uint32_t offset;

    if (!name || !start || !off) {
        return -1;
    }

    offset = fdt_subnode_offset(fdt, 0, name);
    if (offset <= 0) {
        log_error("%s NULL.\r\n", name);
        return -1;
    }

    *start = (uint32_t) fdt;
    *off = offset;

    return 0;
}

static void __opt_feature_init(void) {
#ifdef CONF_USER_ENABLE_VFS_ROMFS
    romfs_register();
#endif
}

static void aos_loop_proc(void *pvParameters) {
    int fd_console;
    uint32_t fdt = 0, offset = 0;
    static StackType_t proc_stack_looprt[512];
    static StaticTask_t proc_task_looprt;

    /*Init bloop stuff*/
    looprt_start(proc_stack_looprt, 512, &proc_task_looprt);
    loopset_led_hook_on_looprt();

    easyflash_init();
    vfs_init();
    vfs_device_init();

    if (0 == get_dts_addr("uart", &fdt, &offset)) {
        vfs_uart_init(fdt, offset);
    }

    if (0 == get_dts_addr("gpio", &fdt, &offset)) {
        hal_gpio_init_from_dts(fdt, offset);
    }

    if (0 == get_dts_addr("pwm", &fdt, &offset)) {
        vfs_pwm_init(fdt, offset);
    }


    __opt_feature_init();
    aos_loop_init();

    fd_console = aos_open("/dev/ttyS0", 0);
    if (fd_console >= 0) {
        printf("Init CLI with event Driven\r\n");
        aos_cli_init(0);
        aos_poll_read_fd(fd_console, aos_cli_event_cb_read_get(), (void *) 0x12345678);
        _cli_init();
    }

    aos_register_event_filter(EV_WIFI, event_cb_wifi_event, NULL);
    cmd_stack_wifi(NULL, 0, 0, NULL);

    aos_loop_run();
    vTaskDelete(NULL);
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vAssertCalled(void) {
    volatile uint32_t ulSetTo1ToExitFunction = 0;

    taskDISABLE_INTERRUPTS();
    while (ulSetTo1ToExitFunction != 1) {
        __asm volatile( "NOP" );
    }
}


//TCP CLIENT



static void tcp_client_err(void *arg, err_t err) {
    log_error("connect error! close by core!\r\n");
    log_error("try to connect to server again!\r\n");
    tcp_close(client_pcb);

    tcp_client_raw_init();
}

char tcp_client_send(void *arg, uint32_t pic_len) {
    tcp_write(client_pcb, arg, pic_len, NETCONN_COPY);
    tcp_output(client_pcb);
    return ERR_OK;
}

char tcp_client_write(void *arg, uint32_t pic_len) {
    tcp_write(client_pcb, arg, pic_len, NETCONN_COPY);
    return ERR_OK;
}

char tcp_client_output() {
    tcp_output(client_pcb);
    return ERR_OK;
}

static err_t tcp_client_recv(void *arg, struct tcp_pcb *t_pcb, struct pbuf *p, err_t err) {
    if (p != NULL) {
        tcp_recved(t_pcb, p->tot_len);
        char *temp = strtok(p->payload, "@");
        int i = 0;
        int type = -1;
        int number = -1;
        // void *data = NULL;

        while (temp != NULL) {
            if (type == -1 && i == 0) {
                if (temp[0] == 'R' && temp[1] == 'T') {
                    type = RECV_DATA_TYPE_WEATHER;
                    // data = malloc(sizeof(WeatherData));
                    // memset(data, 0x00, sizeof(WeatherData));
                    continue;
                } else {
                    printf("ERROR\r\n");
                    break;
                }
            } else {
                if (type == RECV_DATA_TYPE_WEATHER) {
                    if (i == 1) {
                        number = atoi(temp);
                    }
                }
            }
            temp = strtok(NULL, "@");
            i++;
        }
        printf("number: %d\r\n",number);
        if (type == RECV_DATA_TYPE_WEATHER && number > 0 && number < 5) {
            blog_info("Start PWM %d\r\n",number);
            bl_pwm_init(0, 20, 3200);
            bl_pwm_set_duty(0, 50);
            bl_pwm_start(0);
            aos_msleep(1900 * number);
            bl_pwm_stop(0);
            blog_info("STOP PWM\r\n");
        }
        memset(p->payload, 0, p->tot_len);
        free(temp);
        pbuf_free(p);
    } else if (err == ERR_OK) {
        // disconnect server
        log_error("Disconnect from server!\r\n");
        tcp_close(t_pcb);

        // reconnect
        tcp_client_raw_init();
    }
    return ERR_OK;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err) {
    printf("Connected to server successfully!\r\n");
    tcp_recv(pcb, tcp_client_recv);

    // tcp_write(client_pcb, send_buf, sizeof(send_buf), NETCONN_COPY);
    // tcp_output(client_pcb);

    return ERR_OK;
}

void tcp_client_raw_init(void) {
    weather_data = alloca(sizeof(WeatherData));
    ip4_addr_t server_ip;
    client_pcb = tcp_new();
    if (client_pcb == NULL) {
        log_error("Creat client pcb failed!\r\n");
        return;
    }
    IP4_ADDR(&server_ip, SERVER_IP_ADDR0, SERVER_IP_ADDR1, SERVER_IP_ADDR2, SERVER_IP_ADDR3);
    printf("Start connect server!\r\n");
    tcp_connect(client_pcb, &server_ip, TCP_CLIENT_TEST_PORT, tcp_client_connected);
    tcp_err(client_pcb, tcp_client_err);
}



#define DHT11_PORT 7
uint8_t dht11_val;

void DHT11_Start(void) {
    bl_gpio_output_set(DHT11_PORT, 0);
    aos_msleep(19);
    bl_gpio_input_get(DHT11_PORT, &dht11_val);

    while (dht11_val == 0) {
        printf("1: %d\r\n", dht11_val);
        bl_gpio_input_get(DHT11_PORT, &dht11_val);
    }
    while (dht11_val == 1) {
        printf("2: %d\r\n", dht11_val);
        bl_gpio_input_get(DHT11_PORT, &dht11_val);
    }
}


uint8_t DHT11_ReadValue(void) {
    uint8_t i, sbuf = 0;
    for (i = 8; i > 0; i--) {
        sbuf <<= 1;
        bl_gpio_input_get(DHT11_PORT, &dht11_val);
        while (dht11_val == 0) {
            printf("3: %d\r\n", dht11_val);
            bl_gpio_input_get(DHT11_PORT, &dht11_val);
        }
        for (uint8_t i = 0; i < 5; i++) {
            bl_gpio_input_get(DHT11_PORT, &dht11_val);
        }
        if (dht11_val == 1) {
            sbuf |= 1;
        } else {
            sbuf |= 0;
        }

        bl_gpio_input_get(DHT11_PORT, &dht11_val);
        while (dht11_val == 1) {
            printf("4: %d\r\n", dht11_val);
            bl_gpio_input_get(DHT11_PORT, &dht11_val);
        }
    }
    return sbuf;
}


uint8_t DHT11_ReadTemHum(uint8_t *buf) {
    uint8_t check;

    buf[0] = DHT11_ReadValue();
    buf[1] = DHT11_ReadValue();
    buf[2] = DHT11_ReadValue();
    buf[3] = DHT11_ReadValue();

    check = DHT11_ReadValue();

    if (check == buf[0] + buf[1] + buf[2] + buf[3])
        return 1;
    else
        return 0;
}


static void system_init(void) {
    blog_init();
    bl_irq_init();
    bl_sec_init();
    bl_sec_test();
    bl_dma_init();
    hal_boot2_init();

    hal_board_cfg(0);
}

static void system_thread_init() {

}

void bfl_main() {
    static StackType_t aos_loop_proc_stack[1024];
    static StaticTask_t aos_loop_proc_task;
    static StackType_t proc_hellow_stack[512];
    static StaticTask_t proc_hellow_task;

    time_main = bl_timer_now_us();

    bl_uart_init(0, 16, 7, 255, 255, 2 * 1000 * 1000);
    puts("Starting bl602 now....\r\n");

    bl_sys_init();

    vPortDefineHeapRegions(xHeapRegions);
    printf("Heap %u@%p, %u@%p\r\n",
           (unsigned int) &_heap_size, &_heap_start,
           (unsigned int) &_heap_wifi_size, &_heap_wifi_start
    );
    printf("Boot2 consumed %lums\r\n", time_main / 1000);

    system_init();
    system_thread_init();

    puts("[OS] Starting aos_loop_proc task...\r\n");
    xTaskCreateStatic(aos_loop_proc, (char *) "event_loop", 1024, NULL, 15, aos_loop_proc_stack, &aos_loop_proc_task);
    puts("[OS] Starting TCP/IP Stack...\r\n");
    tcpip_init(NULL, NULL);

    puts("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();
}
