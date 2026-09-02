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
/**
 * Represent the integration execution policy data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationExecutionPolicy {
    unsigned timeout_ms;
    unsigned maximum_attempts;
    unsigned initial_backoff_ms;
    unsigned circuit_failure_threshold;
    bool stop_on_mapping_error;
} UmiIntegrationExecutionPolicy;
/**
 * Provide the integration execution policy default operation used by this module and its
 * client applications.
 */
void umi_integration_execution_policy_default(UmiIntegrationExecutionPolicy *policy);
/**
 * Check that integration execution policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_integration_execution_policy_validate(const UmiIntegrationExecutionPolicy *policy,UmiIntegrationDesignerValidation *validation);
/**
 * Provide the integration execution policy backoff operation used by this module and its
 * client applications.
 */
unsigned umi_integration_execution_policy_backoff(const UmiIntegrationExecutionPolicy *policy,unsigned attempt);
#endif
