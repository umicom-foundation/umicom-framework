/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/history.h
 *
 * PURPOSE:
 *   Retain regression-run history and baseline comparisons.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_HISTORY
#define UMICOM_TEST_RUNTIME_HISTORY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime history data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeHistory {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t run_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeHistory;

/**
 * Initialise test runtime history from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_runtime_history_init(UmiTestRuntimeHistory *value, const char *id);
/**
 * Check that test runtime history satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_history_validate(const UmiTestRuntimeHistory *value);
/**
 * Provide the test runtime history set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_history_set_name(UmiTestRuntimeHistory *value, const char *name);
/**
 * Provide the test runtime history set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_history_set_detail(UmiTestRuntimeHistory *value, const char *detail);
/**
 * Return the number of records represented by test runtime history set run without
 * changing their state.
 */
UmiStatus umi_test_runtime_history_set_run_count(UmiTestRuntimeHistory *value, uint64_t number);
/**
 * Provide the test runtime history set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_history_set_generation(UmiTestRuntimeHistory *value, uint64_t number);
/**
 * Provide the test runtime history touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_history_touch(UmiTestRuntimeHistory *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime history same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_history_same_identity(const UmiTestRuntimeHistory *left, const UmiTestRuntimeHistory *right);

#ifdef __cplusplus
}
#endif
#endif
