/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/sanitizer_evidence.h
 *
 * PURPOSE:
 *   Retain sanitizer type, finding count and first failing location.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SANITIZER_EVIDENCE
#define UMICOM_TEST_RUNTIME_SANITIZER_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeSanitizerEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t finding_count;
    uint64_t suppressed_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeSanitizerEvidence;
void umi_test_runtime_sanitizer_evidence_init(UmiTestRuntimeSanitizerEvidence *value,const char *id);
UmiStatus umi_test_runtime_sanitizer_evidence_validate(const UmiTestRuntimeSanitizerEvidence *value);
UmiStatus umi_test_runtime_sanitizer_evidence_set_category(UmiTestRuntimeSanitizerEvidence *value,const char *category);
UmiStatus umi_test_runtime_sanitizer_evidence_set_detail(UmiTestRuntimeSanitizerEvidence *value,const char *detail);
UmiStatus umi_test_runtime_sanitizer_evidence_set_finding_count(UmiTestRuntimeSanitizerEvidence *value,uint64_t number);
UmiStatus umi_test_runtime_sanitizer_evidence_set_suppressed_count(UmiTestRuntimeSanitizerEvidence *value,uint64_t number);
UmiStatus umi_test_runtime_sanitizer_evidence_set_active(UmiTestRuntimeSanitizerEvidence *value,bool active);
bool umi_test_runtime_sanitizer_evidence_same_identity(const UmiTestRuntimeSanitizerEvidence *left,const UmiTestRuntimeSanitizerEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
