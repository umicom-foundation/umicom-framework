/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/event.c
 *
 * PURPOSE:
 *   Implement a bounded FIFO for normalised frontend input events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/event.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"


struct UmiUiEventQueue { UmiUiEvent events[UMI_UI_EVENT_QUEUE_MAX]; size_t count; uint64_t next_id; UmiMutex *mutex; };
/*
 * Initialise ui event queue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_event_queue_create(UmiUiEventQueue **out){UmiUiEventQueue *q;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;q=calloc(1U,sizeof(*q));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&q->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK){free(q);return s;}q->next_id=1U;*out=q;return UMI_STATUS_OK;}
/* Release or reset state held by ui event queue so the same storage can be reused safely. */
void umi_ui_event_queue_destroy(UmiUiEventQueue *q){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL)return;umi_mutex_destroy(q->mutex);free(q);}
/* Provide the ui event push operation used by this module and its client applications. */
UmiStatus umi_ui_event_push(UmiUiEventQueue *q,const UmiUiEvent *e,uint64_t *out_id){UmiUiEvent v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL||e==NULL||!umi_ui_id_is_valid(e->source_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(q->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->count>=UMI_UI_EVENT_QUEUE_MAX){(void)umi_mutex_unlock(q->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}v=*e;v.event_id=q->next_id++;q->events[q->count++]=v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_id!=NULL)*out_id=v.event_id;(void)umi_mutex_unlock(q->mutex);return UMI_STATUS_OK;}
/* Provide the ui event pop operation used by this module and its client applications. */
UmiStatus umi_ui_event_pop(UmiUiEventQueue *q,UmiUiEvent *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(q->mutex);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->count==0U){(void)umi_mutex_unlock(q->mutex);return UMI_STATUS_NOT_FOUND;}*out=q->events[0];(void)memmove(&q->events[0],&q->events[1],(q->count-1U)*sizeof(q->events[0]));q->count--; (void)umi_mutex_unlock(q->mutex);return UMI_STATUS_OK;}
/* Return the number of records represented by ui event without changing their state. */
size_t umi_ui_event_count(const UmiUiEventQueue *q){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q!=NULL){(void)umi_mutex_lock(q->mutex);n=q->count;(void)umi_mutex_unlock(q->mutex);}return n;}
/* Release or reset state held by ui event so the same storage can be reused safely. */
void umi_ui_event_clear(UmiUiEventQueue *q){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL)return;(void)umi_mutex_lock(q->mutex);q->count=0U;(void)umi_mutex_unlock(q->mutex);}
