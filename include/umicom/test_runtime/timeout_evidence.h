/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/timeout_evidence.h
 *
 * PURPOSE:
 *   Retain timeout budgets, elapsed time and cancellation evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TIMEOUT_EVIDENCE
#define UMICOM_TEST_RUNTIME_TIMEOUT_EVIDENCE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeTimeoutEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t elapsed_ms;
    uint64_t budget_ms;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeTimeoutEvidence;

void umi_test_runtime_timeout_evidence_init(UmiTestRuntimeTimeoutEvidence *value, const char *id);
UmiStatus umi_test_runtime_timeout_evidence_validate(const UmiTestRuntimeTimeoutEvidence *value);
UmiStatus umi_test_runtime_timeout_evidence_set_name(UmiTestRuntimeTimeoutEvidence *value, const char *name);
UmiStatus umi_test_runtime_timeout_evidence_set_detail(UmiTestRuntimeTimeoutEvidence *value, const char *detail);
UmiStatus umi_test_runtime_timeout_evidence_set_elapsed_ms(UmiTestRuntimeTimeoutEvidence *value, uint64_t number);
UmiStatus umi_test_runtime_timeout_evidence_set_budget_ms(UmiTestRuntimeTimeoutEvidence *value, uint64_t number);
UmiStatus umi_test_runtime_timeout_evidence_touch(UmiTestRuntimeTimeoutEvidence *value, uint64_t updated_at_ms);
bool umi_test_runtime_timeout_evidence_same_identity(const UmiTestRuntimeTimeoutEvidence *left, const UmiTestRuntimeTimeoutEvidence *right);

#ifdef __cplusplus
}
#endif
#endif
