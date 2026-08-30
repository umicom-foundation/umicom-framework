/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/test_family.h
 *
 * PURPOSE:
 *   Group tests by durable subsystem identity for targeted execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TEST_FAMILY
#define UMICOM_TEST_RUNTIME_TEST_FAMILY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeTestFamily
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t member_count;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeTestFamily;
void umi_test_runtime_test_family_init(UmiTestRuntimeTestFamily *value,const char *id);
UmiStatus umi_test_runtime_test_family_validate(const UmiTestRuntimeTestFamily *value);
UmiStatus umi_test_runtime_test_family_set_detail(UmiTestRuntimeTestFamily *value,const char *detail);
UmiStatus umi_test_runtime_test_family_set_member_count(UmiTestRuntimeTestFamily *value,uint64_t number);
UmiStatus umi_test_runtime_test_family_set_generation(UmiTestRuntimeTestFamily *value,uint64_t number);
bool umi_test_runtime_test_family_same_identity(const UmiTestRuntimeTestFamily *left,const UmiTestRuntimeTestFamily *right);
#ifdef __cplusplus
}
#endif
#endif
