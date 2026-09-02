/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/runtime_path.h
 *
 * PURPOSE:
 *   Retain one normalised runtime search path with provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUNTIME_PATH
#define UMICOM_TEST_RUNTIME_RUNTIME_PATH
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime runtime path data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeRuntimePath {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t priority;
    uint64_t source_kind;
    uint64_t revision;
    bool active;
} UmiTestRuntimeRuntimePath;
/**
 * Initialise test runtime runtime path from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_runtime_path_init(UmiTestRuntimeRuntimePath *value,const char *id);
/**
 * Check that test runtime runtime path satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_path_validate(const UmiTestRuntimeRuntimePath *value);
/**
 * Provide the test runtime runtime path set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_category(UmiTestRuntimeRuntimePath *value,const char *category);
/**
 * Provide the test runtime runtime path set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_detail(UmiTestRuntimeRuntimePath *value,const char *detail);
/**
 * Provide the test runtime runtime path set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_priority(UmiTestRuntimeRuntimePath *value,uint64_t number);
/**
 * Provide the test runtime runtime path set source kind operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_source_kind(UmiTestRuntimeRuntimePath *value,uint64_t number);
/**
 * Provide the test runtime runtime path set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_path_set_active(UmiTestRuntimeRuntimePath *value,bool active);
/**
 * Provide the test runtime runtime path same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_runtime_path_same_identity(const UmiTestRuntimeRuntimePath *left,const UmiTestRuntimeRuntimePath *right);
#ifdef __cplusplus
}
#endif
#endif
