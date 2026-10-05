/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/server.c
 *
 * PURPOSE:
 *   Implement native web-server lifecycle around the platform listener.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/server.h"
#include <stdlib.h>
#include <string.h>
#include "server_connection_internal.h"
/* The service is borrowed and the listener is owned by this lifecycle. serving
 * prevents a callback from recursively entering a second socket exchange. */
struct UmiWebServer{UmiWebServerConfig config;UmiWebService *service;UmiWebListener listener;UmiWebServerState state; int serving;};
/*
 * Initialise web server from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_web_server_create(const UmiWebServerConfig *config,UmiWebService *service,UmiWebServer **out_server){UmiWebServer *s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(config==NULL||service==NULL||out_server==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_web_server_config_validate(config)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;*out_server=NULL;s=(UmiWebServer*)calloc(1U,sizeof(*s));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;s->config=*config;s->service=service;umi_web_server_state_init(&s->state);s->state.port=config->port;*out_server=s;return UMI_STATUS_OK;}
/* Release or reset state held by web server so the same storage can be reused safely. */
void umi_web_server_destroy(UmiWebServer *server){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(server==NULL)return;(void)umi_web_server_stop(server);free(server);}
/* Provide the web server start operation used by this module and its client applications. */
UmiStatus umi_web_server_start(UmiWebServer *server){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(server==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(server->state.phase==UMI_WEB_SERVER_READY)return UMI_STATUS_ALREADY_EXISTS;server->state.phase=UMI_WEB_SERVER_STARTING;s=umi_web_listener_open(&server->config,&server->listener);server->state.last_status=s;server->state.phase=s==UMI_STATUS_OK?UMI_WEB_SERVER_READY:UMI_WEB_SERVER_FAILED;return s;}
/* Provide the web server stop operation used by this module and its client applications. */
UmiStatus umi_web_server_stop(UmiWebServer *server){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(server==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(server->state.phase==UMI_WEB_SERVER_STOPPED)return UMI_STATUS_OK;server->state.phase=UMI_WEB_SERVER_STOPPING;umi_web_listener_close(&server->listener);server->state.phase=UMI_WEB_SERVER_STOPPED;server->state.last_status=UMI_STATUS_OK;return UMI_STATUS_OK;}
/* Provide the web server state operation used by this module and its client applications. */
const UmiWebServerState *umi_web_server_state(const UmiWebServer *server){return server!=NULL?&server->state:NULL;}

/* Routing stays with the existing service. This entry point adds the missing
 * byte-stream exchange without changing legacy start/stop or in-memory APIs. */
UmiStatus UmiWebServerServeNext(UmiWebServer *server,uint32_t wait_ms,uint32_t exchange_ms,
    const UmiCancellationToken *cancel,UmiWebExchangeResult *out)
{
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));
    if(server==NULL||wait_ms==0U||wait_ms>60000U||exchange_ms==0U||exchange_ms>60000U)return UMI_STATUS_INVALID_ARGUMENT;
    if(server->serving)return UMI_STATUS_BUSY;
    if(server->state.phase!=UMI_WEB_SERVER_READY||!server->listener.open)return UMI_STATUS_INVALID_STATE;
    if(server->config.loopback_only!=1||strcmp(server->config.bind_address,"127.0.0.1")!=0)return UMI_STATUS_PERMISSION_DENIED;
    server->serving=1;
    UmiStatus status=WebNativeServeNext(&server->listener,server->service,server->config.max_request_bytes,
        wait_ms,exchange_ms,cancel,out);
    server->serving=0;server->state.last_status=status;
    if(out->dispatched&&server->state.requests!=UINT64_MAX)++server->state.requests;
    return status;
}
