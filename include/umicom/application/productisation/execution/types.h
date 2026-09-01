/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/types.h
 *
 * PURPOSE:
 *   Define execution-control states and bounded capacities for completion-plan delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_TYPES_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/application/productisation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_EXECUTION_MAX_ITEMS UMI_PRODUCTISATION_MAX_STEPS
#define UMI_PRODUCT_EXECUTION_MAX_DEPENDENCIES 2048U
#define UMI_PRODUCT_EXECUTION_MAX_EVENTS 2048U
#define UMI_PRODUCT_EXECUTION_MAX_CHECKPOINTS 256U
#define UMI_PRODUCT_EXECUTION_MAX_ROLLBACKS 768U
#define UMI_PRODUCT_EXECUTION_LABEL_CAPACITY 192U
#define UMI_PRODUCT_EXECUTION_DETAIL_CAPACITY 384U

typedef enum UmiProductExecutionState {
    UMI_PRODUCT_EXECUTION_PENDING = 1,
    UMI_PRODUCT_EXECUTION_READY = 2,
    UMI_PRODUCT_EXECUTION_RUNNING = 3,
    UMI_PRODUCT_EXECUTION_SUCCEEDED = 4,
    UMI_PRODUCT_EXECUTION_FAILED = 5,
    UMI_PRODUCT_EXECUTION_BLOCKED = 6,
    UMI_PRODUCT_EXECUTION_CANCELLED = 7,
    UMI_PRODUCT_EXECUTION_ROLLED_BACK = 8
} UmiProductExecutionState;

typedef enum UmiProductExecutionOutcome {
    UMI_PRODUCT_EXECUTION_OUTCOME_NONE = 0,
    UMI_PRODUCT_EXECUTION_OUTCOME_SUCCESS = 1,
    UMI_PRODUCT_EXECUTION_OUTCOME_RETRYABLE_FAILURE = 2,
    UMI_PRODUCT_EXECUTION_OUTCOME_PERMANENT_FAILURE = 3,
    UMI_PRODUCT_EXECUTION_OUTCOME_BLOCKED = 4,
    UMI_PRODUCT_EXECUTION_OUTCOME_CANCELLED = 5
} UmiProductExecutionOutcome;

typedef enum UmiProductExecutionEventKind {
    UMI_PRODUCT_EXECUTION_EVENT_QUEUED = 1,
    UMI_PRODUCT_EXECUTION_EVENT_READY = 2,
    UMI_PRODUCT_EXECUTION_EVENT_STARTED = 3,
    UMI_PRODUCT_EXECUTION_EVENT_SUCCEEDED = 4,
    UMI_PRODUCT_EXECUTION_EVENT_FAILED = 5,
    UMI_PRODUCT_EXECUTION_EVENT_BLOCKED = 6,
    UMI_PRODUCT_EXECUTION_EVENT_CANCELLED = 7,
    UMI_PRODUCT_EXECUTION_EVENT_CHECKPOINT = 8,
    UMI_PRODUCT_EXECUTION_EVENT_ROLLBACK = 9
} UmiProductExecutionEventKind;

/** Return stable diagnostic text for a work-item lifecycle state. */
const char *umi_product_execution_state_text(UmiProductExecutionState state);
/** Return stable diagnostic text for an execution outcome. */
const char *umi_product_execution_outcome_text(UmiProductExecutionOutcome outcome);
/** Return stable diagnostic text for a recorded history event. */
const char *umi_product_execution_event_kind_text(UmiProductExecutionEventKind kind);
/** Return non-zero when no further lifecycle transition is expected. */
int umi_product_execution_state_terminal(UmiProductExecutionState state);
/** Return non-zero when the lifecycle permits the requested state change. */
int umi_product_execution_state_can_transition(
    UmiProductExecutionState current,
    UmiProductExecutionState next);

#ifdef __cplusplus
}
#endif

#endif
