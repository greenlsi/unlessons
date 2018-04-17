#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_common.h"
#include "esp_wifi.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"
#include "raft.h"

#include "http_request.h"
#include "http_response.h"
#include "http_router.h"

fsm_t* fsm_new_wifi (const char* ssid, const char* pw);
fsm_t* fsm_new_httpd (int port);
void httpd_add_route (uint8_t* route, void (*func) (Request *, Response *));

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}


static void pong (Request *request, Response *response) {
    response_write (response, "pong");
}

static void
raftd (void* ignore)
{
    fsm_t* wifi_fsm = fsm_new_wifi ("SSID", "password");
    fsm_t* httpd_fsm = fsm_new_httpd (80);
    fsm_t* raft_fsm = fsm_new_raft (10000, 1);

    httpd_add_route ("/ping", pong);

    portTickType period =  250 /portTICK_RATE_MS;
    portTickType last = xTaskGetTickCount();
    while (1) {
        fsm_fire (wifi_fsm);
        fsm_fire (httpd_fsm);
        fsm_fire (raft_fsm);
        vTaskDelayUntil (&last, period);
    }
}


void
user_init (void)
{
    vTaskDelay (10000 /portTICK_RATE_MS);

    xTaskHandle task_raftd;
    xTaskCreate (raftd, "raftd", 2048, NULL,  5, &task_raftd);
}
