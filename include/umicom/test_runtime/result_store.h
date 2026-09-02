/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/result_store.h
 *
 * PURPOSE:
 *   Retain bounded result history for diagnostics and trend analysis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESULT_STORE
#define UMICOM_TEST_RUNTIME_RESULT_STORE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime result store data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeResultStore {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t result_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeResultStore;

/**
 * Initialise test runtime result store from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_result_store_init(UmiTestRuntimeResultStore *value, const char *id);
/**
 * Check that test runtime result store satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_result_store_validate(const UmiTestRuntimeResultStore *value);
/**
 * Provide the test runtime result store set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_result_store_set_name(UmiTestRuntimeResultStore *value, const char *name);
/**
 * Provide the test runtime result store set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_result_store_set_detail(UmiTestRuntimeResultStore *value, const char *detail);
/**
 * Return the number of records represented by test runtime result store set result without
 * changing their state.
 */
UmiStatus umi_test_runtime_result_store_set_result_count(UmiTestRuntimeResultStore *value, uint64_t number);
/**
 * Provide the test runtime result store set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_result_store_set_generation(UmiTestRuntimeResultStore *value, uint64_t number);
/**
 * Provide the test runtime result store touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_result_store_touch(UmiTestRuntimeResultStore *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime result store same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_result_store_same_identity(const UmiTestRuntimeResultStore *left, const UmiTestRuntimeResultStore *right);

#ifdef __cplusplus
}
#endif
#endif
