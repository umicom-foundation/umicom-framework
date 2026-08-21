/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/memory_evidence.h
 *
 * PURPOSE:
 *   Retain allocation, bounds and ownership diagnostics for test failures.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_MEMORY_EVIDENCE
#define UMICOM_TEST_RUNTIME_MEMORY_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeMemoryEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t allocation_count;
    uint64_t failure_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeMemoryEvidence;
void umi_test_runtime_memory_evidence_init(UmiTestRuntimeMemoryEvidence *value,const char *id);
UmiStatus umi_test_runtime_memory_evidence_validate(const UmiTestRuntimeMemoryEvidence *value);
UmiStatus umi_test_runtime_memory_evidence_set_category(UmiTestRuntimeMemoryEvidence *value,const char *category);
UmiStatus umi_test_runtime_memory_evidence_set_detail(UmiTestRuntimeMemoryEvidence *value,const char *detail);
UmiStatus umi_test_runtime_memory_evidence_set_allocation_count(UmiTestRuntimeMemoryEvidence *value,uint64_t number);
UmiStatus umi_test_runtime_memory_evidence_set_failure_count(UmiTestRuntimeMemoryEvidence *value,uint64_t number);
UmiStatus umi_test_runtime_memory_evidence_set_active(UmiTestRuntimeMemoryEvidence *value,bool active);
bool umi_test_runtime_memory_evidence_same_identity(const UmiTestRuntimeMemoryEvidence *left,const UmiTestRuntimeMemoryEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
