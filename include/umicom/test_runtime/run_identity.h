/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/run_identity.h
 *
 * PURPOSE:
 *   Provide stable identity for one complete regression execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUN_IDENTITY
#define UMICOM_TEST_RUNTIME_RUN_IDENTITY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeRunIdentity
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t sequence;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeRunIdentity;
void umi_test_runtime_run_identity_init(UmiTestRuntimeRunIdentity *value,const char *id);
UmiStatus umi_test_runtime_run_identity_validate(const UmiTestRuntimeRunIdentity *value);
UmiStatus umi_test_runtime_run_identity_set_detail(UmiTestRuntimeRunIdentity *value,const char *detail);
UmiStatus umi_test_runtime_run_identity_set_sequence(UmiTestRuntimeRunIdentity *value,uint64_t number);
UmiStatus umi_test_runtime_run_identity_set_generation(UmiTestRuntimeRunIdentity *value,uint64_t number);
bool umi_test_runtime_run_identity_same_identity(const UmiTestRuntimeRunIdentity *left,const UmiTestRuntimeRunIdentity *right);
#ifdef __cplusplus
}
#endif
#endif
