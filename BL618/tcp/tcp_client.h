#ifndef __TCP_CLIENT__H__
#define __TCP_CLIENT__H__

#define TCP_CLIENT_TEST_PORT (7361)

#define SERVER_IP_ADDR0 (192)
#define SERVER_IP_ADDR1 (168)
#define SERVER_IP_ADDR2 (3)
#define SERVER_IP_ADDR3 (114)

#define RECV_DATA_TYPE_WEATHER (1)
#define RECV_DATA_TYPE_SETTING (2)

static const uint8_t send_buf[7] = {
        'w', 'e', 'a', 't', 'h', 'e', 'r'
};

static const uint8_t *BOARD_UUID = "UUID:dec74884-b6fb-42fc-980d-b4c5a6be0cee";

char tcp_client_send(void *arg, uint32_t pic_len) ;
void tcp_client_raw_init(void);
char tcp_client_write(void *arg, uint32_t pic_len);
char tcp_client_output();

typedef struct{
    uint8_t province[128];
    uint8_t weather[64];
    uint8_t temperature;
    uint8_t windDirection[64];
    uint8_t windPower[64];
    uint8_t humidity;
}WeatherData ;


#endif
