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
void umi_test_runtime_network_requirement_init(UmiTestRuntimeNetworkRequirement *value,const char *id);
UmiStatus umi_test_runtime_network_requirement_validate(const UmiTestRuntimeNetworkRequirement *value);
UmiStatus umi_test_runtime_network_requirement_set_detail(UmiTestRuntimeNetworkRequirement *value,const char *detail);
UmiStatus umi_test_runtime_network_requirement_set_required(UmiTestRuntimeNetworkRequirement *value,uint64_t number);
UmiStatus umi_test_runtime_network_requirement_set_isolated(UmiTestRuntimeNetworkRequirement *value,uint64_t number);
bool umi_test_runtime_network_requirement_same_identity(const UmiTestRuntimeNetworkRequirement *left,const UmiTestRuntimeNetworkRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
