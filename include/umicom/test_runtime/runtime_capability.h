/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/runtime_capability.h
 *
 * PURPOSE:
 *   Describe one capability required before a test can be launched.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUNTIME_CAPABILITY
#define UMICOM_TEST_RUNTIME_RUNTIME_CAPABILITY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeRuntimeCapability
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t available;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeRuntimeCapability;
void umi_test_runtime_runtime_capability_init(UmiTestRuntimeRuntimeCapability *value,const char *id);
UmiStatus umi_test_runtime_runtime_capability_validate(const UmiTestRuntimeRuntimeCapability *value);
UmiStatus umi_test_runtime_runtime_capability_set_detail(UmiTestRuntimeRuntimeCapability *value,const char *detail);
UmiStatus umi_test_runtime_runtime_capability_set_required(UmiTestRuntimeRuntimeCapability *value,uint64_t number);
UmiStatus umi_test_runtime_runtime_capability_set_available(UmiTestRuntimeRuntimeCapability *value,uint64_t number);
bool umi_test_runtime_runtime_capability_same_identity(const UmiTestRuntimeRuntimeCapability *left,const UmiTestRuntimeRuntimeCapability *right);
#ifdef __cplusplus
}
#endif
#endif
