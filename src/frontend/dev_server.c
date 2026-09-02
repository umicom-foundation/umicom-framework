/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/dev_server.c
 *
 * PURPOSE:
 *   Implement the dev server behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/dev_server.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the frontend dev server config default operation used by this module and its
 * client applications.
 */
UmiFrontendDevServerConfig umi_frontend_dev_server_config_default(void){UmiFrontendDevServerConfig c={0};(void)umi_frontend_dev_copy_text(c.root,sizeof(c.root),".");(void)umi_frontend_dev_copy_text(c.host,sizeof(c.host),"127.0.0.1");c.port=4310U;c.single_page_fallback=1;return c;}
/*
 * Initialise frontend dev server from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_frontend_dev_server_init(UmiFrontendDevServer *s,const UmiFrontendDevServerConfig *c){int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||c==NULL||c->root[0]=='\0'||c->host[0]=='\0'||c->port==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));s->config=*c;n=snprintf(s->base_url,sizeof(s->base_url),"http://%s:%u",c->host,(unsigned)c->port);if(n<0||(size_t)n>=sizeof(s->base_url))return UMI_STATUS_CAPACITY_EXCEEDED;s->generation=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend dev server start operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_dev_server_start(UmiFrontendDevServer *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->running)return UMI_STATUS_ALREADY_EXISTS;s->running=1;s->generation+=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend dev server stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_dev_server_stop(UmiFrontendDevServer *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!s->running)return UMI_STATUS_INVALID_STATE;s->running=0;s->generation+=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend dev server record request operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_dev_server_record_request(UmiFrontendDevServer *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!s->running)return UMI_STATUS_INVALID_STATE;s->request_count+=1U;return UMI_STATUS_OK;}
