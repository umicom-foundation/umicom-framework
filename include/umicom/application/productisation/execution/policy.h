/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/policy.h
 *
 * PURPOSE:
 *   Define execution guardrails for Framework-first ownership, retries and acceptance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_POLICY_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_POLICY_H
#include "umicom/application/productisation/execution/work_queue.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionPolicy {
    unsigned maximum_attempts;
    int dry_run;
    int allow_application_adoption;
    int allow_external_adapters;
    int require_framework_completion_first;
    int require_acceptance_after_delivery;
} UmiProductExecutionPolicy;
/** Return the conservative default limits for a product execution run. */
UmiProductExecutionPolicy umi_product_execution_policy_default(void);
/** Validate retry, concurrency, ownership and mutation policy values. */
UmiStatus umi_product_execution_policy_validate(
    const UmiProductExecutionPolicy *policy);
/** Decide whether one work item may run under the supplied policy. */
UmiStatus umi_product_execution_policy_check_item(
    const UmiProductExecutionPolicy *policy,
    const UmiProductExecutionWorkQueue *queue,
    size_t item_index);
#ifdef __cplusplus
}
#endif
#endif
