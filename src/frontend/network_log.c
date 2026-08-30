/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/network_log.c
 *
 * PURPOSE:
 *   Implement the network log behavior for
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
#include "umicom/frontend/network_log.h"
#include <string.h>
UmiStatus umi_frontend_network_log_init(UmiFrontendNetworkLog *l){if(l==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(l,0,sizeof(*l));l->next_sequence=1U;return UMI_STATUS_OK;}
UmiStatus umi_frontend_network_log_append(UmiFrontendNetworkLog *l,const UmiFrontendNetworkEntry *e){size_t p;if(l==NULL||e==NULL||e->url[0]=='\0'||e->method[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;if(l->count<UMI_FRONTEND_DEV_MAX_NETWORK_ENTRIES)p=l->count++;else{(void)memmove(&l->items[0],&l->items[1],(UMI_FRONTEND_DEV_MAX_NETWORK_ENTRIES-1U)*sizeof(l->items[0]));p=UMI_FRONTEND_DEV_MAX_NETWORK_ENTRIES-1U;}l->items[p]=*e;l->items[p].sequence=l->next_sequence++;return UMI_STATUS_OK;}
UmiStatus umi_frontend_network_log_at(const UmiFrontendNetworkLog *l,size_t i,UmiFrontendNetworkEntry *out){if(l==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(i>=l->count)return UMI_STATUS_NOT_FOUND;*out=l->items[i];return UMI_STATUS_OK;}
void umi_frontend_network_log_clear(UmiFrontendNetworkLog *l){if(l==NULL)return;l->count=0U;}
