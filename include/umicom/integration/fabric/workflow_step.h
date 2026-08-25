/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/workflow_step.h
 *
 * PURPOSE:
 *   Describe an orchestrated integration step with timeout and compensation metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_WORKFLOW_STEP_H
#define UMICOM_INTEGRATION_FABRIC_WORKFLOW_STEP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricWorkflowStep {
    char step_id[UMI_FABRIC_ID_CAPACITY];
    char operation_id[UMI_FABRIC_ID_CAPACITY];
    uint64_t timeout_ms;
    bool optional;
    bool compensatable;
} UmiFabricWorkflowStep;

UmiStatus umi_fabric_workflow_step_init(UmiFabricWorkflowStep *item, const char *step_id, const char *operation_id, uint64_t timeout_ms, bool optional, bool compensatable);
UmiStatus umi_fabric_workflow_step_validate(const UmiFabricWorkflowStep *item);

#ifdef __cplusplus
}
#endif
#endif
