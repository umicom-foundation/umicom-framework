/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/temporary_directory.h
 *
 * PURPOSE:
 *   Describe isolated temporary state used by one test execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TEMPORARY_DIRECTORY
#define UMICOM_TEST_RUNTIME_TEMPORARY_DIRECTORY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime temporary directory data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeTemporaryDirectory {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t created;
    uint64_t cleanup_required;
    uint64_t revision;
    bool active;
} UmiTestRuntimeTemporaryDirectory;
/**
 * Initialise test runtime temporary directory from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_temporary_directory_init(UmiTestRuntimeTemporaryDirectory *value,const char *id);
/**
 * Check that test runtime temporary directory satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_temporary_directory_validate(const UmiTestRuntimeTemporaryDirectory *value);
/**
 * Provide the test runtime temporary directory set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_temporary_directory_set_category(UmiTestRuntimeTemporaryDirectory *value,const char *category);
/**
 * Provide the test runtime temporary directory set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_temporary_directory_set_detail(UmiTestRuntimeTemporaryDirectory *value,const char *detail);
/**
 * Provide the test runtime temporary directory set created operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_temporary_directory_set_created(UmiTestRuntimeTemporaryDirectory *value,uint64_t number);
/**
 * Provide the test runtime temporary directory set cleanup required operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_temporary_directory_set_cleanup_required(UmiTestRuntimeTemporaryDirectory *value,uint64_t number);
/**
 * Provide the test runtime temporary directory set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_temporary_directory_set_active(UmiTestRuntimeTemporaryDirectory *value,bool active);
/**
 * Provide the test runtime temporary directory same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_temporary_directory_same_identity(const UmiTestRuntimeTemporaryDirectory *left,const UmiTestRuntimeTemporaryDirectory *right);
#ifdef __cplusplus
}
#endif
#endif
