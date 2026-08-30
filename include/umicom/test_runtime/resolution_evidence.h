/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/resolution_evidence.h
 *
 * PURPOSE:
 *   Retain evidence that a previously failing test now passes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESOLUTION_EVIDENCE
#define UMICOM_TEST_RUNTIME_RESOLUTION_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeResolutionEvidence
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t verification_count;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeResolutionEvidence;
void umi_test_runtime_resolution_evidence_init(UmiTestRuntimeResolutionEvidence *value,const char *id);
UmiStatus umi_test_runtime_resolution_evidence_validate(const UmiTestRuntimeResolutionEvidence *value);
UmiStatus umi_test_runtime_resolution_evidence_set_detail(UmiTestRuntimeResolutionEvidence *value,const char *detail);
UmiStatus umi_test_runtime_resolution_evidence_set_verification_count(UmiTestRuntimeResolutionEvidence *value,uint64_t number);
UmiStatus umi_test_runtime_resolution_evidence_set_generation(UmiTestRuntimeResolutionEvidence *value,uint64_t number);
bool umi_test_runtime_resolution_evidence_same_identity(const UmiTestRuntimeResolutionEvidence *left,const UmiTestRuntimeResolutionEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
