/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/filesystem_requirement.h
 *
 * PURPOSE:
 *   Describe writable/read-only filesystem requirements and roots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FILESYSTEM_REQUIREMENT
#define UMICOM_TEST_RUNTIME_FILESYSTEM_REQUIREMENT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeFilesystemRequirement
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t available;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeFilesystemRequirement;
void umi_test_runtime_filesystem_requirement_init(UmiTestRuntimeFilesystemRequirement *value,const char *id);
UmiStatus umi_test_runtime_filesystem_requirement_validate(const UmiTestRuntimeFilesystemRequirement *value);
UmiStatus umi_test_runtime_filesystem_requirement_set_detail(UmiTestRuntimeFilesystemRequirement *value,const char *detail);
UmiStatus umi_test_runtime_filesystem_requirement_set_required(UmiTestRuntimeFilesystemRequirement *value,uint64_t number);
UmiStatus umi_test_runtime_filesystem_requirement_set_available(UmiTestRuntimeFilesystemRequirement *value,uint64_t number);
bool umi_test_runtime_filesystem_requirement_same_identity(const UmiTestRuntimeFilesystemRequirement *left,const UmiTestRuntimeFilesystemRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
