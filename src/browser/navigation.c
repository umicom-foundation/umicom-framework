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

/* BEGINNER NOTE:
 * The embedded-browser layer stays provider neutral. A future WebKit, Edge or Qt adapter can implement the same function table.
 */

#include "umicom/browser/navigation.h"
#include "umicom/web/types.h"
#include <string.h>
void umi_browser_history_init(UmiBrowserHistory *h){if(h!=NULL)(void)memset(h,0,sizeof(*h));}
UmiStatus umi_browser_history_visit(UmiBrowserHistory *h,const char *url){if(h==NULL||url==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(h->count>0U&&h->current+1U<h->count)h->count=h->current+1U;if(h->count>=UMI_BROWSER_HISTORY_CAPACITY){(void)memmove(&h->entries[0],&h->entries[1],(UMI_BROWSER_HISTORY_CAPACITY-1U)*UMI_BROWSER_URL_CAPACITY);h->count=UMI_BROWSER_HISTORY_CAPACITY-1U;if(h->current>0U)--h->current;}if(umi_web_copy_text(h->entries[h->count],UMI_BROWSER_URL_CAPACITY,url)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;h->current=h->count;++h->count;return UMI_STATUS_OK;}
const char *umi_browser_history_back(UmiBrowserHistory *h){if(h==NULL||h->count==0U||h->current==0U)return NULL;--h->current;return h->entries[h->current];}
const char *umi_browser_history_forward(UmiBrowserHistory *h){if(h==NULL||h->count==0U||h->current+1U>=h->count)return NULL;++h->current;return h->entries[h->current];}
const char *umi_browser_history_current(const UmiBrowserHistory *h){return h!=NULL&&h->count>0U?h->entries[h->current]:NULL;}
