/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/tool_requirement.h
 *
 * PURPOSE:
 *   Describe compiler, debugger or external-tool requirements for tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TOOL_REQUIREMENT
#define UMICOM_TEST_RUNTIME_TOOL_REQUIREMENT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeToolRequirement
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t minimum_version;
    uint64_t available;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeToolRequirement;
void umi_test_runtime_tool_requirement_init(UmiTestRuntimeToolRequirement *value,const char *id);
UmiStatus umi_test_runtime_tool_requirement_validate(const UmiTestRuntimeToolRequirement *value);
UmiStatus umi_test_runtime_tool_requirement_set_detail(UmiTestRuntimeToolRequirement *value,const char *detail);
UmiStatus umi_test_runtime_tool_requirement_set_minimum_version(UmiTestRuntimeToolRequirement *value,uint64_t number);
UmiStatus umi_test_runtime_tool_requirement_set_available(UmiTestRuntimeToolRequirement *value,uint64_t number);
bool umi_test_runtime_tool_requirement_same_identity(const UmiTestRuntimeToolRequirement *left,const UmiTestRuntimeToolRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
