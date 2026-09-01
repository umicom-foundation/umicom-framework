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
static void emit(Headless *h,UmiBrowserEventKind kind,const char *text){UmiBrowserEvent e;if(h->sink==NULL)return;(void)memset(&e,0,sizeof(e));e.kind=kind;(void)umi_web_copy_text(e.text,sizeof(e.text),text!=NULL?text:"");h->sink(&e,h->data);}
static UmiStatus nav(void *p,const char *url){Headless *h=(Headless*)p;UmiStatus s=umi_browser_history_visit(&h->history,url);if(s==UMI_STATUS_OK)emit(h,UMI_BROWSER_NAVIGATED,url);return s;}
static UmiStatus back(void *p){Headless *h=(Headless*)p;const char *u=umi_browser_history_back(&h->history);if(u==NULL)return UMI_STATUS_NOT_FOUND;emit(h,UMI_BROWSER_NAVIGATED,u);return UMI_STATUS_OK;}
static UmiStatus forward(void *p){Headless *h=(Headless*)p;const char *u=umi_browser_history_forward(&h->history);if(u==NULL)return UMI_STATUS_NOT_FOUND;emit(h,UMI_BROWSER_NAVIGATED,u);return UMI_STATUS_OK;}
static UmiStatus reload(void *p){Headless *h=(Headless*)p;const char *u=umi_browser_history_current(&h->history);if(u==NULL)return UMI_STATUS_NOT_FOUND;emit(h,UMI_BROWSER_NAVIGATED,u);return UMI_STATUS_OK;}
static UmiStatus post(void *p,const char *m){Headless *h=(Headless*)p;UmiStatus s=umi_browser_bridge_push(h->bridge,m);if(s==UMI_STATUS_OK)emit(h,UMI_BROWSER_MESSAGE,m);return s;}
static const char *current(const void *p){const Headless *h=(const Headless*)p;return umi_browser_history_current(&h->history);}
static void destroy(void *p){Headless *h=(Headless*)p;if(h==NULL)return;umi_browser_bridge_destroy(h->bridge);free(h);}
UmiStatus umi_browser_headless_create(UmiBrowserEventSink sink,void *data,UmiBrowser *out){Headless *h;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));h=(Headless*)calloc(1U,sizeof(*h));if(h==NULL)return UMI_STATUS_OUT_OF_MEMORY;umi_browser_history_init(&h->history);s=umi_browser_bridge_create(&h->bridge);if(s!=UMI_STATUS_OK){free(h);return s;}h->sink=sink;h->data=data;out->instance=h;out->navigate=nav;out->back=back;out->forward=forward;out->reload=reload;out->post_message=post;out->current_url=current;out->destroy=destroy;return UMI_STATUS_OK;}
