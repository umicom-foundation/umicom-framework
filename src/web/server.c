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
struct UmiWebServer{UmiWebServerConfig config;UmiWebService *service;UmiWebListener listener;UmiWebServerState state;};
UmiStatus umi_web_server_create(const UmiWebServerConfig *config,UmiWebService *service,UmiWebServer **out_server){UmiWebServer *s;if(config==NULL||service==NULL||out_server==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_web_server_config_validate(config)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;*out_server=NULL;s=(UmiWebServer*)calloc(1U,sizeof(*s));if(s==NULL)return UMI_STATUS_OUT_OF_MEMORY;s->config=*config;s->service=service;umi_web_server_state_init(&s->state);s->state.port=config->port;*out_server=s;return UMI_STATUS_OK;}
void umi_web_server_destroy(UmiWebServer *server){if(server==NULL)return;(void)umi_web_server_stop(server);free(server);}
UmiStatus umi_web_server_start(UmiWebServer *server){UmiStatus s;if(server==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(server->state.phase==UMI_WEB_SERVER_READY)return UMI_STATUS_ALREADY_EXISTS;server->state.phase=UMI_WEB_SERVER_STARTING;s=umi_web_listener_open(&server->config,&server->listener);server->state.last_status=s;server->state.phase=s==UMI_STATUS_OK?UMI_WEB_SERVER_READY:UMI_WEB_SERVER_FAILED;return s;}
UmiStatus umi_web_server_stop(UmiWebServer *server){if(server==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(server->state.phase==UMI_WEB_SERVER_STOPPED)return UMI_STATUS_OK;server->state.phase=UMI_WEB_SERVER_STOPPING;umi_web_listener_close(&server->listener);server->state.phase=UMI_WEB_SERVER_STOPPED;server->state.last_status=UMI_STATUS_OK;return UMI_STATUS_OK;}
const UmiWebServerState *umi_web_server_state(const UmiWebServer *server){return server!=NULL?&server->state:NULL;}
