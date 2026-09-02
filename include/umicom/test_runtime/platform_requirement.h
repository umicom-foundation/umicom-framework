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
/**
 * Represent the test runtime platform requirement data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise test runtime platform requirement from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_platform_requirement_init(UmiTestRuntimePlatformRequirement *value,const char *id);
/**
 * Check that test runtime platform requirement satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_platform_requirement_validate(const UmiTestRuntimePlatformRequirement *value);
/**
 * Provide the test runtime platform requirement set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_platform_requirement_set_detail(UmiTestRuntimePlatformRequirement *value,const char *detail);
/**
 * Provide the test runtime platform requirement set architecture bits operation used by
 * this module and its client applications.
 */
UmiStatus umi_test_runtime_platform_requirement_set_architecture_bits(UmiTestRuntimePlatformRequirement *value,uint64_t number);
/**
 * Provide the test runtime platform requirement set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_platform_requirement_set_required(UmiTestRuntimePlatformRequirement *value,uint64_t number);
/**
 * Provide the test runtime platform requirement same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_platform_requirement_same_identity(const UmiTestRuntimePlatformRequirement *left,const UmiTestRuntimePlatformRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
