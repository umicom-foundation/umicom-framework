/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/policy.h
 *
 * PURPOSE:
 *   Define execution guardrails for Framework-first ownership, retries and acceptance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
UmiProductExecutionPolicy umi_product_execution_policy_default(void);
UmiStatus umi_product_execution_policy_validate(
    const UmiProductExecutionPolicy *policy);
UmiStatus umi_product_execution_policy_check_item(
    const UmiProductExecutionPolicy *policy,
    const UmiProductExecutionWorkQueue *queue,
    size_t item_index);
#ifdef __cplusplus
}
#endif
#endif
