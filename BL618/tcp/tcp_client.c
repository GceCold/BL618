/**
 * @file tcp_client.c
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"

#include "lwip/api.h"
#include "lwip/arch.h"
#include "lwip/opt.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "FreeRTOS.h"
#include "task.h"

#define DBG_TAG "TCP"

#include "log.h"

#include "tcp_client.h"

extern int flag1;
extern int flag2;
extern int flag3;
extern int flag4;

static struct tcp_pcb *client_pcb = NULL;

WeatherData *weather_data;

static void tcp_client_err(void *arg, err_t err) {
    LOG_E("connect error! close by core!\r\n");
    LOG_E("try to connect to server again!\r\n");
    tcp_close(client_pcb);

    tcp_client_raw_init();
}

char tcp_client_send(void *arg, uint32_t pic_len) {
    tcp_write(client_pcb, arg, pic_len, NETCONN_COPY);
    tcp_output(client_pcb);
    while (client_pcb->state == SYN_SENT) {
        bflb_mtimer_delay_ms(1);
    }

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
        void *data = NULL;

        while (temp != NULL) {
            if (type == -1 && i == 0) {
                if (temp[0] == 'W' && temp[1] == 'H') {
                    type = RECV_DATA_TYPE_WEATHER;
                    data = malloc(sizeof(WeatherData));
                    memset(data, 0x00, sizeof(WeatherData));
                    continue;
                } else if (temp[0] == 'C' && temp[1] == 'L'){
                    type = RECV_DATA_TYPE_SETTING;
                    continue;
                }else {
                    printf("Unknow data type\r\n");
                    break;
                }
            } else {
                if (type == RECV_DATA_TYPE_WEATHER) {
                    if (i == 1) {
                        sscanf(temp, "%s", ((WeatherData *) data)->province);
                    }
                    if (i == 2) {
                        sscanf(temp, "%s", ((WeatherData *) data)->weather);
                    }
                    if (i == 3) {
                        ((WeatherData *) data)->temperature = strtol(temp, NULL, 10);
                    }
                    if (i == 4) {
                        sscanf(temp, "%s", ((WeatherData *) data)->windDirection);
                    }
                    if (i == 5) {
                        sscanf(temp, "%s", ((WeatherData *) data)->windPower);
                    }
                    if (i == 6) {
                        ((WeatherData *) data)->humidity = strtol(temp, NULL, 10);
                    }
                }
                //配置已放置衣服
                if (type == RECV_DATA_TYPE_SETTING) {
                    if (i == 1) {
                        flag1 = strtol(temp, NULL, 10);
                    }
                    if (i == 2) {
                        flag2 = strtol(temp, NULL, 10);
                    }
                    if (i == 3) {
                        flag3 = strtol(temp, NULL, 10);
                    }
                    if (i == 4) {
                        flag4 = strtol(temp, NULL, 10);
                    }
                }
            }
            temp = strtok(NULL, "@");
            i++;
        }
        if (type == RECV_DATA_TYPE_WEATHER) {
            memcpy(weather_data, data, sizeof(WeatherData));
            LOG_I("Weather: \r\n");
            LOG_I("Province: %s\r\n", weather_data->province);
            LOG_I("Weather: %s\r\n", weather_data->weather);
            LOG_I("Temperature: %d\r\n", weather_data->temperature);
            LOG_I("Winddirection: %s\r\n", weather_data->windDirection);
            LOG_I("Windpower: %s\r\n", weather_data->windPower);
            LOG_I("Humidity: %d\r\n", weather_data->humidity);
            tcp_write(client_pcb, BOARD_UUID, sizeof(BOARD_UUID), NETCONN_COPY);
            tcp_output(client_pcb);
        }
        // LOG_I("recv data: %s\r\n", p->payload);

        memset(p->payload, 0, p->tot_len);
        free(data);
        free(temp);
        pbuf_free(p);
    } else if (err == ERR_OK) {
        // disconnect server
        LOG_E("Disconnect from server!\r\n");
        tcp_close(t_pcb);

        // reconnect
        tcp_client_raw_init();
    }
    return ERR_OK;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err) {
    LOG_I("Connected to server successfully!\r\n");
    tcp_recv(pcb, tcp_client_recv);

    tcp_write(client_pcb, send_buf, sizeof(send_buf), NETCONN_COPY);
    tcp_output(client_pcb);

    return ERR_OK;
}

void tcp_client_raw_init(void) {
    weather_data = alloca(sizeof(WeatherData));
    ip4_addr_t server_ip;
    client_pcb = tcp_new();
    if (client_pcb == NULL) {
        LOG_E("Creat client pcb failed!\r\n");
        return;
    }
    IP4_ADDR(&server_ip, SERVER_IP_ADDR0, SERVER_IP_ADDR1, SERVER_IP_ADDR2, SERVER_IP_ADDR3);
    LOG_I("Start connect server!\r\n");
    tcp_connect(client_pcb, &server_ip, TCP_CLIENT_TEST_PORT, tcp_client_connected);
    tcp_err(client_pcb, tcp_client_err);
}


#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_send_data(int argc, char **argv)
{
    tcp_write(client_pcb, send_buf, sizeof(send_buf), NETCONN_COPY);
    tcp_output(client_pcb);
    LOG_E("Sernd Data;\r\n");
    return 0;
}

int cmd_connect_server(int argc, char **argv)
{
    tcp_client_raw_init();
    return 0;
}


SHELL_CMD_EXPORT_ALIAS(cmd_connect_server, connect_server, connect server);
SHELL_CMD_EXPORT_ALIAS(cmd_send_data, send_data, send data);
#endif
