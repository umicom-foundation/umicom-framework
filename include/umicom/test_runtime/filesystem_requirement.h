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
/**
 * Represent the test runtime filesystem requirement data shared with callers of this
 * public contract.
 */
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
/**
 * Initialise test runtime filesystem requirement from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_filesystem_requirement_init(UmiTestRuntimeFilesystemRequirement *value,const char *id);
/**
 * Check that test runtime filesystem requirement satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_filesystem_requirement_validate(const UmiTestRuntimeFilesystemRequirement *value);
/**
 * Provide the test runtime filesystem requirement set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_filesystem_requirement_set_detail(UmiTestRuntimeFilesystemRequirement *value,const char *detail);
/**
 * Provide the test runtime filesystem requirement set required operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_filesystem_requirement_set_required(UmiTestRuntimeFilesystemRequirement *value,uint64_t number);
/**
 * Provide the test runtime filesystem requirement set available operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_filesystem_requirement_set_available(UmiTestRuntimeFilesystemRequirement *value,uint64_t number);
/**
 * Provide the test runtime filesystem requirement same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_filesystem_requirement_same_identity(const UmiTestRuntimeFilesystemRequirement *left,const UmiTestRuntimeFilesystemRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
