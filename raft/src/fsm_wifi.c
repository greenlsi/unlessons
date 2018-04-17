#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "esp_sta.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"

#include "lwip/sockets.h"
#include "lwip/tcp.h"

#include "http_request.h"
#include "http_response.h"
#include "http_router.h"

static System_Event_t* wifi_event = NULL;
static void wifi_handle_event_cb (System_Event_t* evt) {
    wifi_event = evt;
}

static int always (fsm_t* this) { return 1; }

static int event_connected (fsm_t* this) {
    return wifi_event && (wifi_event->event_id == EVENT_STAMODE_CONNECTED);
}
static int event_disconnected (fsm_t* this) {
    return wifi_event && (wifi_event->event_id == EVENT_STAMODE_DISCONNECTED);
}
static int event_authmode_change (fsm_t* this) {
    return wifi_event && (wifi_event->event_id == EVENT_STAMODE_AUTHMODE_CHANGE);
}
static int event_got_ip (fsm_t* this) {
    return wifi_event && (wifi_event->event_id == EVENT_STAMODE_GOT_IP);
}

static void do_connect (fsm_t* this) { wifi_station_connect (); }
static void clear_event (fsm_t* this) { wifi_event = NULL; }

fsm_t*
fsm_new_wifi (const char* ssid, const char* pw)
{
    static fsm_trans_t tt[] = {
        {  0, always,                1, do_connect },
        {  1, event_connected,       1, clear_event },
        {  1, event_disconnected,    1, clear_event },
        {  1, event_authmode_change, 1, clear_event },
        {  1, event_got_ip,          1, clear_event },
        { -1, NULL, -1, NULL },
    };
    struct station_config config;
    memset (&config, 0, sizeof (config));
    sprintf (config.ssid, ssid);
    sprintf (config.password, pw);
    wifi_set_opmode (STATION_MODE);
    wifi_station_set_config (&config);
    wifi_set_event_handler_cb (wifi_handle_event_cb);
    return fsm_new (tt);
}
