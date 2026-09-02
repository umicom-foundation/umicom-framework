/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/execution_policy.c
 *
 * PURPOSE:
 *   Implement the execution policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Integration execution policy | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/execution_policy.h"

/*
 * Provide the integration execution policy default operation used by this module and its
 * client applications.
 */
void umi_integration_execution_policy_default(UmiIntegrationExecutionPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    policy->timeout_ms = 30000U;
    policy->maximum_attempts = 3U;
    policy->initial_backoff_ms = 250U;
    policy->circuit_failure_threshold = 5U;
    policy->stop_on_mapping_error = true;
}

/*
 * Check that integration execution policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_integration_execution_policy_validate(const UmiIntegrationExecutionPolicy *policy,UmiIntegrationDesignerValidation *validation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    validation->count = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (policy->timeout_ms == 0U || policy->timeout_ms > 600000U) (void)umi_integration_designer_validation_add(validation,"policy.timeout","Timeout must be between 1 and 600000 milliseconds");
    /* Apply this operation only while the related capability or state is available. */
    if (policy->maximum_attempts == 0U || policy->maximum_attempts > 10U) (void)umi_integration_designer_validation_add(validation,"policy.attempts","Attempts must be between 1 and 10");
    /* Apply this operation only while the related capability or state is available. */
    if (policy->circuit_failure_threshold == 0U) (void)umi_integration_designer_validation_add(validation,"policy.circuit","Circuit threshold must be greater than zero");
    return validation->count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the integration execution policy backoff operation used by this module and its
 * client applications.
 */
unsigned umi_integration_execution_policy_backoff(const UmiIntegrationExecutionPolicy *policy,unsigned attempt)
{
    unsigned delay;
    unsigned index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || attempt == 0U) return 0U;
    delay = policy->initial_backoff_ms;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < attempt && delay <= 300000U / 2U; ++index) delay *= 2U;
    return delay > 300000U ? 300000U : delay;
}
