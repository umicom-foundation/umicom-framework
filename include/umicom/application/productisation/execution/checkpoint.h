/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/checkpoint.h
 *
 * PURPOSE:
 *   Record resumable completion checkpoints without persisting application-private state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_CHECKPOINT_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_CHECKPOINT_H
#include "umicom/application/productisation/execution/work_queue.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionCheckpoint {
    uint64_t revision;
    size_t succeeded;
    size_t failed;
    size_t blocked;
    size_t cancelled;
    char label[UMI_PRODUCT_EXECUTION_LABEL_CAPACITY];
} UmiProductExecutionCheckpoint;
typedef struct UmiProductExecutionCheckpointLedger {
    UmiProductExecutionCheckpoint checkpoints[UMI_PRODUCT_EXECUTION_MAX_CHECKPOINTS];
    size_t count;
    uint64_t next_revision;
} UmiProductExecutionCheckpointLedger;
/** Clear the bounded checkpoint ledger before the first capture. */
void umi_product_execution_checkpoint_ledger_init(
    UmiProductExecutionCheckpointLedger *ledger);
/** Capture the queue and history position needed for safe recovery. */
UmiStatus umi_product_execution_checkpoint_capture(
    UmiProductExecutionCheckpointLedger *ledger,
    const UmiProductExecutionWorkQueue *queue,
    const char *label);
/** Borrow the newest checkpoint, or return NULL when none exists. */
const UmiProductExecutionCheckpoint *umi_product_execution_checkpoint_latest(
    const UmiProductExecutionCheckpointLedger *ledger);
#ifdef __cplusplus
}
#endif
#endif
