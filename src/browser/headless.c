/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/headless.c
 *
 * PURPOSE:
 *   Provide a deterministic headless browser for tests and server-side workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The embedded-browser layer stays provider neutral. A future WebKit, Edge or Qt adapter can implement the same function table.
 */

#include "umicom/browser/headless.h"
#include "umicom/browser/navigation.h"
#include "umicom/browser/bridge.h"
#include "umicom/web/types.h"
#include <stdlib.h>
#include <string.h>
typedef struct Headless{UmiBrowserHistory history;UmiBrowserBridge *bridge;UmiBrowserEventSink sink;void *data;}Headless;
/* Provide the emit operation used by this module and its client applications. */
static void emit(Headless *h,UmiBrowserEventKind kind,const char *text){UmiBrowserEvent e;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h->sink==NULL)return;(void)memset(&e,0,sizeof(e));e.kind=kind;(void)umi_web_copy_text(e.text,sizeof(e.text),text!=NULL?text:"");h->sink(&e,h->data);}
/* Provide the nav operation used by this module and its client applications. */
static UmiStatus nav(void *p,const char *url){Headless *h=(Headless*)p;UmiStatus s=umi_browser_history_visit(&h->history,url);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)emit(h,UMI_BROWSER_NAVIGATED,url);return s;}
/* Provide the back operation used by this module and its client applications. */
static UmiStatus back(void *p){Headless *h=(Headless*)p;const char *u=umi_browser_history_back(&h->history);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL)return UMI_STATUS_NOT_FOUND;emit(h,UMI_BROWSER_NAVIGATED,u);return UMI_STATUS_OK;}
/* Provide the forward operation used by this module and its client applications. */
static UmiStatus forward(void *p){Headless *h=(Headless*)p;const char *u=umi_browser_history_forward(&h->history);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL)return UMI_STATUS_NOT_FOUND;emit(h,UMI_BROWSER_NAVIGATED,u);return UMI_STATUS_OK;}
/* Provide the reload operation used by this module and its client applications. */
static UmiStatus reload(void *p){Headless *h=(Headless*)p;const char *u=umi_browser_history_current(&h->history);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u==NULL)return UMI_STATUS_NOT_FOUND;emit(h,UMI_BROWSER_NAVIGATED,u);return UMI_STATUS_OK;}
/* Provide the post operation used by this module and its client applications. */
static UmiStatus post(void *p,const char *m){Headless *h=(Headless*)p;UmiStatus s=umi_browser_bridge_push(h->bridge,m);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)emit(h,UMI_BROWSER_MESSAGE,m);return s;}
/* Provide the current operation used by this module and its client applications. */
static const char *current(const void *p){const Headless *h=(const Headless*)p;return umi_browser_history_current(&h->history);}
/* Provide the destroy operation used by this module and its client applications. */
static void destroy(void *p){Headless *h=(Headless*)p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL)return;umi_browser_bridge_destroy(h->bridge);free(h);}
/*
 * Initialise browser headless from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_browser_headless_create(UmiBrowserEventSink sink,void *data,UmiBrowser *out){Headless *h;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));h=(Headless*)calloc(1U,sizeof(*h));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL)return UMI_STATUS_OUT_OF_MEMORY;umi_browser_history_init(&h->history);s=umi_browser_bridge_create(&h->bridge);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(h);return s;}h->sink=sink;h->data=data;out->instance=h;out->navigate=nav;out->back=back;out->forward=forward;out->reload=reload;out->post_message=post;out->current_url=current;out->destroy=destroy;return UMI_STATUS_OK;}
