/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/platform_requirement.h
 *
 * PURPOSE:
 *   Describe OS and architecture requirements for one test family.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PLATFORM_REQUIREMENT
#define UMICOM_TEST_RUNTIME_PLATFORM_REQUIREMENT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimePlatformRequirement
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t architecture_bits;
    uint64_t required;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimePlatformRequirement;
void umi_test_runtime_platform_requirement_init(UmiTestRuntimePlatformRequirement *value,const char *id);
UmiStatus umi_test_runtime_platform_requirement_validate(const UmiTestRuntimePlatformRequirement *value);
UmiStatus umi_test_runtime_platform_requirement_set_detail(UmiTestRuntimePlatformRequirement *value,const char *detail);
UmiStatus umi_test_runtime_platform_requirement_set_architecture_bits(UmiTestRuntimePlatformRequirement *value,uint64_t number);
UmiStatus umi_test_runtime_platform_requirement_set_required(UmiTestRuntimePlatformRequirement *value,uint64_t number);
bool umi_test_runtime_platform_requirement_same_identity(const UmiTestRuntimePlatformRequirement *left,const UmiTestRuntimePlatformRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
