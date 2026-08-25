/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_operation_queue.c
 *
 * PURPOSE:
 *   Queue bounded Data Server operations with deterministic priority ordering and explicit cancellation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_operation_queue.h"
#include <string.h>

/* Queue reset is allocation-free and safe to rebuild from durable journal evidence. */ void umi_data_data_operation_queue_init(UmiDataOperationQueue *queue){if(queue!=NULL)(void)memset(queue,0,sizeof(*queue));}
/* Push rejects duplicate operation IDs so execution remains idempotently addressable. */ UmiStatus umi_data_data_operation_queue_push(UmiDataOperationQueue *queue,const UmiDataOperation *operation){size_t i;if(queue==NULL||operation==NULL||umi_data_data_operation_validate(operation)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<queue->count;++i)if(strcmp(queue->items[i].operation_id,operation->operation_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(queue->count>=UMI_DATA_ENTERPRISE_MAX_OPERATIONS)return UMI_STATUS_CAPACITY_EXCEEDED;queue->items[queue->count++]=*operation;queue->revision++;return UMI_STATUS_OK;}
/* Pop chooses higher numeric priority, then earlier submission time for deterministic fairness. */ UmiStatus umi_data_data_operation_queue_pop(UmiDataOperationQueue *queue,UmiDataOperation *out_operation){size_t i,best;if(queue==NULL||out_operation==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(queue->count==0U)return UMI_STATUS_NOT_FOUND;best=0U;for(i=1U;i<queue->count;++i)if(queue->items[i].priority>queue->items[best].priority||(queue->items[i].priority==queue->items[best].priority&&queue->items[i].submitted_at<queue->items[best].submitted_at))best=i;*out_operation=queue->items[best];queue->items[best]=queue->items[queue->count-1U];queue->count--;queue->revision++;return UMI_STATUS_OK;}
/* Cancellation is explicit and never terminates an already-running backend operation. */ UmiStatus umi_data_data_operation_queue_cancel(UmiDataOperationQueue *queue,const char *operation_id){size_t i;if(queue==NULL||operation_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<queue->count;++i)if(strcmp(queue->items[i].operation_id,operation_id)==0){if(!queue->items[i].cancellable)return UMI_STATUS_PERMISSION_DENIED;queue->items[i]=queue->items[queue->count-1U];queue->count--;queue->revision++;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
