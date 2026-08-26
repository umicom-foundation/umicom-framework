/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/change_queue.c
 *
 * PURPOSE:
 *   Queue property-change sequence numbers for deterministic dispatch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_queue.h"
#include <string.h>
/* Initialise an empty deterministic change queue. */
void umi_ui_reactive_change_queue_init(UmiUiReactiveChangeQueue *item){ if(item) memset(item,0,sizeof *item); }
/* Append one sequence number while preserving FIFO ordering. */
UmiStatus umi_ui_reactive_change_queue_push(UmiUiReactiveChangeQueue *item,uint64_t sequence){ size_t pos; if(!item||item->count>=UMI_UI_REACTIVE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED; pos=(item->head+item->count)%UMI_UI_REACTIVE_MAX_ITEMS; item->sequences[pos]=sequence; item->count++; return UMI_STATUS_OK; }
/* Remove the oldest queued sequence number. */
UmiStatus umi_ui_reactive_change_queue_pop(UmiUiReactiveChangeQueue *item,uint64_t *out){ if(!item||!out) return UMI_STATUS_INVALID_ARGUMENT; if(item->count==0U) return UMI_STATUS_NOT_FOUND; *out=item->sequences[item->head]; item->head=(item->head+1U)%UMI_UI_REACTIVE_MAX_ITEMS; item->count--; return UMI_STATUS_OK; }
