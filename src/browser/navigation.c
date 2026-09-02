/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/navigation.c
 *
 * PURPOSE:
 *   Maintain deterministic browser navigation history.
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

#include "umicom/browser/navigation.h"
#include "umicom/web/types.h"
#include <string.h>
/*
 * Initialise browser history from caller-provided values so later operations receive a
 * known state.
 */
void umi_browser_history_init(UmiBrowserHistory *h){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h!=NULL)(void)memset(h,0,sizeof(*h));}
/*
 * Provide the browser history visit operation used by this module and its client
 * applications.
 */
UmiStatus umi_browser_history_visit(UmiBrowserHistory *h,const char *url){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL||url==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h->count>0U&&h->current+1U<h->count)h->count=h->current+1U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h->count>=UMI_BROWSER_HISTORY_CAPACITY){(void)memmove(&h->entries[0],&h->entries[1],(UMI_BROWSER_HISTORY_CAPACITY-1U)*UMI_BROWSER_URL_CAPACITY);h->count=UMI_BROWSER_HISTORY_CAPACITY-1U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h->current>0U)--h->current;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_web_copy_text(h->entries[h->count],UMI_BROWSER_URL_CAPACITY,url)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;h->current=h->count;++h->count;return UMI_STATUS_OK;}
/*
 * Provide the browser history back operation used by this module and its client
 * applications.
 */
const char *umi_browser_history_back(UmiBrowserHistory *h){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL||h->count==0U||h->current==0U)return NULL;--h->current;return h->entries[h->current];}
/*
 * Provide the browser history forward operation used by this module and its client
 * applications.
 */
const char *umi_browser_history_forward(UmiBrowserHistory *h){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(h==NULL||h->count==0U||h->current+1U>=h->count)return NULL;++h->current;return h->entries[h->current];}
/*
 * Provide the browser history current operation used by this module and its client
 * applications.
 */
const char *umi_browser_history_current(const UmiBrowserHistory *h){return h!=NULL&&h->count>0U?h->entries[h->current]:NULL;}
