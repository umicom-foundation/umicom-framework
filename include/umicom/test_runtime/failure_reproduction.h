/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_reproduction.h
 *
 * PURPOSE:
 *   Retain the exact runtime context required to reproduce a failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_REPRODUCTION
#define UMICOM_TEST_RUNTIME_FAILURE_REPRODUCTION
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeFailureReproduction
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t evidence_count;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeFailureReproduction;
void umi_test_runtime_failure_reproduction_init(UmiTestRuntimeFailureReproduction *value,const char *id);
UmiStatus umi_test_runtime_failure_reproduction_validate(const UmiTestRuntimeFailureReproduction *value);
UmiStatus umi_test_runtime_failure_reproduction_set_detail(UmiTestRuntimeFailureReproduction *value,const char *detail);
UmiStatus umi_test_runtime_failure_reproduction_set_evidence_count(UmiTestRuntimeFailureReproduction *value,uint64_t number);
UmiStatus umi_test_runtime_failure_reproduction_set_generation(UmiTestRuntimeFailureReproduction *value,uint64_t number);
bool umi_test_runtime_failure_reproduction_same_identity(const UmiTestRuntimeFailureReproduction *left,const UmiTestRuntimeFailureReproduction *right);
#ifdef __cplusplus
}
#endif
#endif
