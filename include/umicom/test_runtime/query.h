/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/query.h
 *
 * PURPOSE:
 *   Describe read-only test-runtime queries and paging boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_QUERY
#define UMICOM_TEST_RUNTIME_QUERY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime query data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeQuery {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t offset;
    uint64_t limit;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeQuery;

/**
 * Initialise test runtime query from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_runtime_query_init(UmiTestRuntimeQuery *value, const char *id);
/**
 * Check that test runtime query satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_query_validate(const UmiTestRuntimeQuery *value);
/**
 * Provide the test runtime query set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_query_set_name(UmiTestRuntimeQuery *value, const char *name);
/**
 * Provide the test runtime query set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_query_set_detail(UmiTestRuntimeQuery *value, const char *detail);
/**
 * Provide the test runtime query set offset operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_query_set_offset(UmiTestRuntimeQuery *value, uint64_t number);
/**
 * Provide the test runtime query set limit operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_query_set_limit(UmiTestRuntimeQuery *value, uint64_t number);
/**
 * Provide the test runtime query touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_query_touch(UmiTestRuntimeQuery *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime query same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_query_same_identity(const UmiTestRuntimeQuery *left, const UmiTestRuntimeQuery *right);

#ifdef __cplusplus
}
#endif
#endif
