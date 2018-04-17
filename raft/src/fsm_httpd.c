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

static Router *router;
static int port;
static int msock;
static int ssock;
static char buffer[256];
static struct sockaddr_in serv_addr;
static struct sockaddr_in cli_addr;

static int httpd_error = 0;
static int error (fsm_t* this) { return httpd_error; }
static int no_error (fsm_t* this) { return !httpd_error; }
static int always (fsm_t* this) { return 1; }

static int
is_wifi_on (fsm_t* this)
{
    return wifi_station_get_connect_status() == STATION_GOT_IP;
}

static int
is_connection_waiting (fsm_t* this)
{
    fd_set fdset;
    struct timeval timeout = {0, 0};
    FD_ZERO (&fdset);
    FD_SET (msock, &fdset);
    return select (msock + 1, &fdset, NULL, NULL, &timeout) > 0;
}

static int
is_request (fsm_t* this)
{
    fd_set fdset;
    struct timeval timeout = {0, 0};
    FD_ZERO (&fdset);
    FD_SET (ssock, &fdset);
    return select (ssock + 1, &fdset, NULL, NULL, &timeout) > 0;
}

static int
is_msock_exception (fsm_t* this)
{
    fd_set fdset;
    struct timeval timeout = {0, 0};
    FD_ZERO (&fdset);
    FD_SET (msock, &fdset);
    return select (msock + 1, NULL, NULL, &fdset, &timeout) > 0;
}

static int
is_ssock_exception (fsm_t* this)
{
    fd_set fdset;
    struct timeval timeout = {0, 0};
    FD_ZERO (&fdset);
    FD_SET (ssock, &fdset);
    return select (ssock + 1, NULL, NULL, &fdset, &timeout) > 0;
}

static void
do_socket (fsm_t* this)
{
    msock = socket (AF_INET, SOCK_STREAM, 0);
    httpd_error = msock < 0;
}

static void
do_bind (fsm_t* this)
{
    memset (&serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons (port);

    httpd_error = bind (msock, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
}

static void
do_listen (fsm_t* this)
{
    listen (msock, 500);
}

static void
do_accept_connection (fsm_t* this)
{
    unsigned clilen = sizeof (cli_addr);
    ssock = accept (msock, (struct sockaddr *) &cli_addr, &clilen);
    httpd_error = ssock < 0;
}

static void
do_read_request (fsm_t* this)
{
    int n = read (ssock, buffer, 255);
    httpd_error = n < 0;
}

static void
do_route_request (fsm_t* this)
{
    route (ssock, router, buffer);
}

static void
do_close_connection (fsm_t* this)
{
    close (ssock);
    ssock = -1;
}



static void client_write (uint8_t *data) {
    if (ssock < 0) return;
    size_t size = strlen((char *) data);
    uint8_t res = write (ssock, (void *) data, size * sizeof(uint8_t));
}



fsm_t*
fsm_new_httpd (int p)
{
    static fsm_trans_t tt[] = {
        {  0,  is_wifi_on,            1,  do_socket },
        {  1,  error,                 0,  NULL },
        {  1,  no_error,              2,  do_bind },
        {  2,  error,                 0,  NULL },
        {  2,  no_error,              10, do_listen },
        {  10, is_connection_waiting, 20, do_accept_connection },
        {  10, is_msock_exception,    0,  do_close_connection },
        {  20, is_request,            21, do_read_request },
        {  21, no_error,              22, do_route_request },
        {  21, error,                 10, do_close_connection },
        {  22, always,                10, do_close_connection },
        {  20, is_ssock_exception,    10, do_close_connection },
        { -1, NULL, -1, NULL },
    };
    port = p;
    msock = ssock = -1;
    router = router_create ();
    router->write = client_write;
    return fsm_new (tt);
}

void httpd_add_route (uint8_t* route, void (*func) (Request *, Response *)) {
    router_add_route (router, route, func);
}
