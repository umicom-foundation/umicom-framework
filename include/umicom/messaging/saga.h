/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/saga.h
 *
 * PURPOSE:
 *   Declare a durable-saga state model that records workflow identity, correlation, progress and compensation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_SAGA_H
#define UMICOM_MESSAGING_SAGA_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/workflow.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiSagaState {
    UMI_SAGA_CREATED = 1,
    UMI_SAGA_RUNNING = 2,
    UMI_SAGA_COMPLETED = 3,
    UMI_SAGA_COMPENSATED = 4,
    UMI_SAGA_FAILED = 5
} UmiSagaState;

typedef struct UmiSaga {
    uint64_t saga_id;
    uint64_t correlation_id;
    const UmiWorkflow *workflow;
    UmiSagaState state;
    UmiWorkflowResult result;
} UmiSaga;

void umi_saga_init(UmiSaga *saga,
                   uint64_t saga_id,
                   uint64_t correlation_id,
                   const UmiWorkflow *workflow);
UmiStatus umi_saga_execute(UmiSaga *saga, void *workflow_context);
const char *umi_saga_state_text(UmiSagaState state);

#ifdef __cplusplus
}
#endif

#endif
