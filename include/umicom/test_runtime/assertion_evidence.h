/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/assertion_evidence.h
 *
 * PURPOSE:
 *   Retain assertion failure identity and source evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ASSERTION_EVIDENCE
#define UMICOM_TEST_RUNTIME_ASSERTION_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeAssertionEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t line;
    uint64_t occurrence_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeAssertionEvidence;
void umi_test_runtime_assertion_evidence_init(UmiTestRuntimeAssertionEvidence *value,const char *id);
UmiStatus umi_test_runtime_assertion_evidence_validate(const UmiTestRuntimeAssertionEvidence *value);
UmiStatus umi_test_runtime_assertion_evidence_set_category(UmiTestRuntimeAssertionEvidence *value,const char *category);
UmiStatus umi_test_runtime_assertion_evidence_set_detail(UmiTestRuntimeAssertionEvidence *value,const char *detail);
UmiStatus umi_test_runtime_assertion_evidence_set_line(UmiTestRuntimeAssertionEvidence *value,uint64_t number);
UmiStatus umi_test_runtime_assertion_evidence_set_occurrence_count(UmiTestRuntimeAssertionEvidence *value,uint64_t number);
UmiStatus umi_test_runtime_assertion_evidence_set_active(UmiTestRuntimeAssertionEvidence *value,bool active);
bool umi_test_runtime_assertion_evidence_same_identity(const UmiTestRuntimeAssertionEvidence *left,const UmiTestRuntimeAssertionEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
