/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/repair_policy.h
 *
 * PURPOSE:
 *   Make validation-failure handling explicit and reusable: accept, retry with
 *   evidence, rollback and retry, or fail.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_REPAIR_POLICY_H
#define UMICOM_AI_CODING_RUNTIME_REPAIR_POLICY_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiAiCodingRepairDecision {
    UMI_AI_CODING_REPAIR_ACCEPT = 0,
    UMI_AI_CODING_REPAIR_RETRY = 1,
    UMI_AI_CODING_REPAIR_ROLLBACK_AND_RETRY = 2,
    UMI_AI_CODING_REPAIR_FAIL = 3
} UmiAiCodingRepairDecision;

typedef struct UmiAiCodingRepairPolicy {
    uint32_t maximum_iterations;
    int rollback_before_retry;
    int retry_optional_failures;
    int retry_required_failures;
} UmiAiCodingRepairPolicy;

void umi_ai_coding_repair_policy_init(UmiAiCodingRepairPolicy *policy);

UmiAiCodingRepairDecision umi_ai_coding_repair_decide(
    const UmiAiCodingRepairPolicy *policy,
    const UmiAiCodingValidationReport *report,
    uint32_t current_iteration);

#ifdef __cplusplus
}
#endif
#endif
