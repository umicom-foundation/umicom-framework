/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_operation_queue.h
 *
 * PURPOSE:
 *   Queue bounded Data Server operations with deterministic priority ordering and explicit cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_OPERATION_QUEUE_H
#define UMICOM_DATA_ENTERPRISE_DATA_OPERATION_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/data_operation.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataOperationQueue { UmiDataOperation items[UMI_DATA_ENTERPRISE_MAX_OPERATIONS]; size_t count; uint64_t revision; } UmiDataOperationQueue;
/* Reset operation queue state. */ void umi_data_data_operation_queue_init(UmiDataOperationQueue *queue);
/* Enqueue a unique operation and retain stable submission evidence. */ UmiStatus umi_data_data_operation_queue_push(UmiDataOperationQueue *queue,const UmiDataOperation *operation);
/* Pop the highest-priority oldest operation. */ UmiStatus umi_data_data_operation_queue_pop(UmiDataOperationQueue *queue,UmiDataOperation *out_operation);
/* Cancel a queued operation when its descriptor permits cancellation. */ UmiStatus umi_data_data_operation_queue_cancel(UmiDataOperationQueue *queue,const char *operation_id);

#ifdef __cplusplus
}
#endif
#endif
