/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/runtime_manifest.h
 *
 * PURPOSE:
 *   Describe the expected runtime files for one test executable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUNTIME_MANIFEST
#define UMICOM_TEST_RUNTIME_RUNTIME_MANIFEST
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime runtime manifest data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeRuntimeManifest {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t file_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeRuntimeManifest;
/**
 * Initialise test runtime runtime manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_runtime_manifest_init(UmiTestRuntimeRuntimeManifest *value,const char *id);
/**
 * Check that test runtime runtime manifest satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_manifest_validate(const UmiTestRuntimeRuntimeManifest *value);
/**
 * Provide the test runtime runtime manifest set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_category(UmiTestRuntimeRuntimeManifest *value,const char *category);
/**
 * Provide the test runtime runtime manifest set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_detail(UmiTestRuntimeRuntimeManifest *value,const char *detail);
/**
 * Return the number of records represented by test runtime runtime manifest set file
 * without changing their state.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_file_count(UmiTestRuntimeRuntimeManifest *value,uint64_t number);
/**
 * Provide the test runtime runtime manifest set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_generation(UmiTestRuntimeRuntimeManifest *value,uint64_t number);
/**
 * Provide the test runtime runtime manifest set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_manifest_set_active(UmiTestRuntimeRuntimeManifest *value,bool active);
/**
 * Provide the test runtime runtime manifest same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_runtime_manifest_same_identity(const UmiTestRuntimeRuntimeManifest *left,const UmiTestRuntimeRuntimeManifest *right);
#ifdef __cplusplus
}
#endif
#endif
