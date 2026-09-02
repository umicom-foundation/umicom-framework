/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/deduplication_window.c
 *
 * PURPOSE:
 *   Maintain a bounded recent-message window for duplicate detection before side effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/deduplication_window.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric deduplication window from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_deduplication_window_init(UmiFabricDeduplicationWindow *window,uint64_t retention_ms){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(window!=NULL){(void)memset(window,0,sizeof(*window));window->retention_ms=retention_ms;}}
/*
 * Provide the fabric deduplication window observe operation used by this module and its
 * client applications.
 */
UmiStatus umi_fabric_deduplication_window_observe(UmiFabricDeduplicationWindow *window,const char *message_id,uint64_t now_ms,bool *out_duplicate){size_t i,slot;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(window==NULL||message_id==NULL||out_duplicate==NULL||window->retention_ms==0U)return UMI_STATUS_INVALID_ARGUMENT;*out_duplicate=false;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<window->count;++i){UmiFabricDedupeEntry *e=&window->entries[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e->message_id,message_id)==0 && now_ms>=e->seen_ms && now_ms-e->seen_ms<=window->retention_ms){*out_duplicate=true;return UMI_STATUS_OK;}}slot=window->count<UMI_FABRIC_MAX_DEDUPE?window->count++:window->cursor;window->cursor=(slot+1U)%UMI_FABRIC_MAX_DEDUPE;(void)memset(&window->entries[slot],0,sizeof(window->entries[slot]));s=umi_fabric_copy_text(window->entries[slot].message_id,sizeof(window->entries[slot].message_id),message_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;window->entries[slot].seen_ms=now_ms;return UMI_STATUS_OK;}
