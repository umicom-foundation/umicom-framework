/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/execution_policy.h
 *
 * PURPOSE:
 *   Publish the public execution policy contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_EXECUTION_POLICY_H
#define UMICOM_INTEGRATION_EXECUTION_POLICY_H
#include "umicom/integration/designer_types.h"
typedef struct UmiIntegrationExecutionPolicy {
    unsigned timeout_ms;
    unsigned maximum_attempts;
    unsigned initial_backoff_ms;
    unsigned circuit_failure_threshold;
    bool stop_on_mapping_error;
} UmiIntegrationExecutionPolicy;
void umi_integration_execution_policy_default(UmiIntegrationExecutionPolicy *policy);
UmiStatus umi_integration_execution_policy_validate(const UmiIntegrationExecutionPolicy *policy,UmiIntegrationDesignerValidation *validation);
unsigned umi_integration_execution_policy_backoff(const UmiIntegrationExecutionPolicy *policy,unsigned attempt);
#endif
