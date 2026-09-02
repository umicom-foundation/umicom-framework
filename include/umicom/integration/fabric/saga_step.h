/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/saga_step.h
 *
 * PURPOSE:
 *   Describe a saga action and its compensation operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SAGA_STEP_H
#define UMICOM_INTEGRATION_FABRIC_SAGA_STEP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric saga step data shared with callers of this public contract.
 */
typedef struct UmiFabricSagaStep {
    char step_id[UMI_FABRIC_ID_CAPACITY];
    char action_operation[UMI_FABRIC_ID_CAPACITY];
    char compensation_operation[UMI_FABRIC_ID_CAPACITY];
    bool compensation_required;
} UmiFabricSagaStep;

/**
 * Initialise fabric saga step from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_fabric_saga_step_init(UmiFabricSagaStep *item, const char *step_id, const char *action_operation, const char *compensation_operation, bool compensation_required);
/**
 * Check that fabric saga step satisfies its contract before another service relies on it.
 */
UmiStatus umi_fabric_saga_step_validate(const UmiFabricSagaStep *item);

#ifdef __cplusplus
}
#endif
#endif
