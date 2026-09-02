/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/working_directory.h
 *
 * PURPOSE:
 *   Validate and retain the working directory selected for a test process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_WORKING_DIRECTORY
#define UMICOM_TEST_RUNTIME_WORKING_DIRECTORY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime working directory data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeWorkingDirectory {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t exists;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeWorkingDirectory;

/**
 * Initialise test runtime working directory from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_working_directory_init(UmiTestRuntimeWorkingDirectory *value, const char *id);
/**
 * Check that test runtime working directory satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_working_directory_validate(const UmiTestRuntimeWorkingDirectory *value);
/**
 * Provide the test runtime working directory set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_working_directory_set_name(UmiTestRuntimeWorkingDirectory *value, const char *name);
/**
 * Provide the test runtime working directory set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_working_directory_set_detail(UmiTestRuntimeWorkingDirectory *value, const char *detail);
/**
 * Provide the test runtime working directory set exists operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_working_directory_set_exists(UmiTestRuntimeWorkingDirectory *value, uint64_t number);
/**
 * Provide the test runtime working directory set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_working_directory_set_generation(UmiTestRuntimeWorkingDirectory *value, uint64_t number);
/**
 * Provide the test runtime working directory touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_working_directory_touch(UmiTestRuntimeWorkingDirectory *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime working directory same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_working_directory_same_identity(const UmiTestRuntimeWorkingDirectory *left, const UmiTestRuntimeWorkingDirectory *right);

#ifdef __cplusplus
}
#endif
#endif
