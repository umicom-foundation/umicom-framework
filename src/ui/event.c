/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/event.c
 *
 * PURPOSE:
 *   Implement a bounded FIFO for normalised frontend input events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/event.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"


struct UmiUiEventQueue { UmiUiEvent events[UMI_UI_EVENT_QUEUE_MAX]; size_t count; uint64_t next_id; UmiMutex *mutex; };
UmiStatus umi_ui_event_queue_create(UmiUiEventQueue **out){UmiUiEventQueue *q;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;q=calloc(1U,sizeof(*q));if(q==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_mutex_create(&q->mutex);if(s!=UMI_STATUS_OK){free(q);return s;}q->next_id=1U;*out=q;return UMI_STATUS_OK;}
void umi_ui_event_queue_destroy(UmiUiEventQueue *q){if(q==NULL)return;umi_mutex_destroy(q->mutex);free(q);}
UmiStatus umi_ui_event_push(UmiUiEventQueue *q,const UmiUiEvent *e,uint64_t *out_id){UmiUiEvent v;if(q==NULL||e==NULL||!umi_ui_id_is_valid(e->source_id))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(q->mutex);if(q->count>=UMI_UI_EVENT_QUEUE_MAX){(void)umi_mutex_unlock(q->mutex);return UMI_STATUS_CAPACITY_EXCEEDED;}v=*e;v.event_id=q->next_id++;q->events[q->count++]=v;if(out_id!=NULL)*out_id=v.event_id;(void)umi_mutex_unlock(q->mutex);return UMI_STATUS_OK;}
UmiStatus umi_ui_event_pop(UmiUiEventQueue *q,UmiUiEvent *out){if(q==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)umi_mutex_lock(q->mutex);if(q->count==0U){(void)umi_mutex_unlock(q->mutex);return UMI_STATUS_NOT_FOUND;}*out=q->events[0];(void)memmove(&q->events[0],&q->events[1],(q->count-1U)*sizeof(q->events[0]));q->count--; (void)umi_mutex_unlock(q->mutex);return UMI_STATUS_OK;}
size_t umi_ui_event_count(const UmiUiEventQueue *q){size_t n=0U;if(q!=NULL){(void)umi_mutex_lock(q->mutex);n=q->count;(void)umi_mutex_unlock(q->mutex);}return n;}
void umi_ui_event_clear(UmiUiEventQueue *q){if(q==NULL)return;(void)umi_mutex_lock(q->mutex);q->count=0U;(void)umi_mutex_unlock(q->mutex);}
