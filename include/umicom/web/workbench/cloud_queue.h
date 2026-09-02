/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/cloud_queue.h
 *
 * PURPOSE:
 *   Project cloud queue and topic state into bounded administration panels with
 *   explicit destructive-operation planning and approval flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_CLOUD_QUEUE_H
#define UMICOM_WEB_WORKBENCH_CLOUD_QUEUE_H

#include "umicom/web/workbench/cloud_object.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named web workbench cloud queue kind values accepted by this public contract.
 */
typedef enum UmiWebWorkbenchCloudQueueKind {
    UMI_WEB_WORKBENCH_CLOUD_QUEUE = 0,
    UMI_WEB_WORKBENCH_CLOUD_TOPIC = 1,
    UMI_WEB_WORKBENCH_CLOUD_DEAD_LETTER = 2
} UmiWebWorkbenchCloudQueueKind;

/**
 * Represent the web workbench cloud queue data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchCloudQueue {
    char queue_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    UmiWebWorkbenchCloudQueueKind kind;
    uint64_t visible_messages;
    uint64_t in_flight_messages;
    uint64_t delayed_messages;
    uint32_t retention_seconds;
    bool fifo;
    bool encrypted;
} UmiWebWorkbenchCloudQueue;

/**
 * Represent the web workbench cloud queue model data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchCloudQueueModel {
    UmiWebWorkbenchCloudProfile profile;
    UmiWebWorkbenchCloudQueue queues[UMI_WEB_WORKBENCH_MAX_CLOUD_ITEMS];
    size_t queue_count;
    size_t omitted_count;
    uint64_t revision;
} UmiWebWorkbenchCloudQueueModel;

/**
 * Initialise web workbench cloud queue model from caller-provided values so later
 * operations receive a known state.
 */
void umi_web_workbench_cloud_queue_model_init(
    UmiWebWorkbenchCloudQueueModel *model,
    const UmiWebWorkbenchCloudProfile *profile);
/**
 * Provide the web workbench cloud queue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_cloud_queue_upsert(
    UmiWebWorkbenchCloudQueueModel *model,
    const UmiWebWorkbenchCloudQueue *queue);
/**
 * Remove web workbench cloud queue while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_web_workbench_cloud_queue_remove(
    UmiWebWorkbenchCloudQueueModel *model,
    const char *queue_id,
    bool approved);
/**
 * Provide the web workbench cloud queue query operation used by this module and its client
 * applications.
 */
size_t umi_web_workbench_cloud_queue_query(
    const UmiWebWorkbenchCloudQueueModel *model,
    const char *text,
    UmiWebWorkbenchCloudQueueKind kind,
    bool filter_kind,
    const UmiWebWorkbenchCloudQueue **out_queues,
    size_t capacity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_CLOUD_QUEUE_H */
