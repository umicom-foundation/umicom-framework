/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/event_queue.c
 *
 * PURPOSE:
 *   Queue browser UI events in bounded FIFO order for Framework-side dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/event_queue.h"

#include <string.h>
/*
 * Initialise native web event queue from caller-provided values so later operations
 * receive a known state.
 */
void umi_native_web_event_queue_init(UmiNativeWebEventQueue *queue){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(queue!=NULL)(void)memset(queue,0,sizeof(*queue));}
/*
 * Provide the native web event queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_event_queue_push(UmiNativeWebEventQueue *queue,const UmiNativeWebUiEvent *event){size_t idx;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(queue==NULL||event==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(queue->count>=UMI_NATIVE_WEB_MAX_EVENTS){++queue->dropped;return UMI_STATUS_CAPACITY_EXCEEDED;}idx=(queue->head+queue->count)%UMI_NATIVE_WEB_MAX_EVENTS;queue->items[idx]=*event;++queue->count;return UMI_STATUS_OK;}
/*
 * Provide the native web event queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_event_queue_pop(UmiNativeWebEventQueue *queue,UmiNativeWebUiEvent *out_event){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(queue==NULL||out_event==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(queue->count==0U)return UMI_STATUS_NOT_FOUND;*out_event=queue->items[queue->head];queue->head=(queue->head+1U)%UMI_NATIVE_WEB_MAX_EVENTS;--queue->count;return UMI_STATUS_OK;}

