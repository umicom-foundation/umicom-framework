/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/network_requirement.h
 *
 * PURPOSE:
 *   Describe whether a test requires an isolated or external network.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_NETWORK_REQUIREMENT
#define UMICOM_TEST_RUNTIME_NETWORK_REQUIREMENT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime network requirement data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeNetworkRequirement
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t isolated;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeNetworkRequirement;
/**
 * Initialise test runtime network requirement from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_network_requirement_init(UmiTestRuntimeNetworkRequirement *value,const char *id);
/**
 * Check that test runtime network requirement satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_network_requirement_validate(const UmiTestRuntimeNetworkRequirement *value);
/**
 * Provide the test runtime network requirement set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_network_requirement_set_detail(UmiTestRuntimeNetworkRequirement *value,const char *detail);
/**
 * Provide the test runtime network requirement set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_network_requirement_set_required(UmiTestRuntimeNetworkRequirement *value,uint64_t number);
/**
 * Provide the test runtime network requirement set isolated operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_network_requirement_set_isolated(UmiTestRuntimeNetworkRequirement *value,uint64_t number);
/**
 * Provide the test runtime network requirement same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_network_requirement_same_identity(const UmiTestRuntimeNetworkRequirement *left,const UmiTestRuntimeNetworkRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
