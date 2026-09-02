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
/**
 * Represent the test runtime runtime capability data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise test runtime runtime capability from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_runtime_capability_init(UmiTestRuntimeRuntimeCapability *value,const char *id);
/**
 * Check that test runtime runtime capability satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_capability_validate(const UmiTestRuntimeRuntimeCapability *value);
/**
 * Provide the test runtime runtime capability set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_capability_set_detail(UmiTestRuntimeRuntimeCapability *value,const char *detail);
/**
 * Provide the test runtime runtime capability set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_runtime_capability_set_required(UmiTestRuntimeRuntimeCapability *value,uint64_t number);
/**
 * Provide the test runtime runtime capability set available operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_runtime_capability_set_available(UmiTestRuntimeRuntimeCapability *value,uint64_t number);
/**
 * Provide the test runtime runtime capability same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_runtime_capability_same_identity(const UmiTestRuntimeRuntimeCapability *left,const UmiTestRuntimeRuntimeCapability *right);
#ifdef __cplusplus
}
#endif
#endif
