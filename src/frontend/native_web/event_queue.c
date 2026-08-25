/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/event_queue.c
 *
 * PURPOSE:
 *   Queue browser UI events in bounded FIFO order for Framework-side dispatch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/event_queue.h"

#include <string.h>
void umi_native_web_event_queue_init(UmiNativeWebEventQueue *queue){if(queue!=NULL)(void)memset(queue,0,sizeof(*queue));}
UmiStatus umi_native_web_event_queue_push(UmiNativeWebEventQueue *queue,const UmiNativeWebUiEvent *event){size_t idx;if(queue==NULL||event==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(queue->count>=UMI_NATIVE_WEB_MAX_EVENTS){++queue->dropped;return UMI_STATUS_CAPACITY_EXCEEDED;}idx=(queue->head+queue->count)%UMI_NATIVE_WEB_MAX_EVENTS;queue->items[idx]=*event;++queue->count;return UMI_STATUS_OK;}
UmiStatus umi_native_web_event_queue_pop(UmiNativeWebEventQueue *queue,UmiNativeWebUiEvent *out_event){if(queue==NULL||out_event==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(queue->count==0U)return UMI_STATUS_NOT_FOUND;*out_event=queue->items[queue->head];queue->head=(queue->head+1U)%UMI_NATIVE_WEB_MAX_EVENTS;--queue->count;return UMI_STATUS_OK;}

