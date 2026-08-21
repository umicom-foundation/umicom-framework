/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/query.h
 *
 * PURPOSE:
 *   Describe read-only test-runtime queries and paging boundaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_QUERY
#define UMICOM_TEST_RUNTIME_QUERY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_test_runtime_query_init(UmiTestRuntimeQuery *value, const char *id);
UmiStatus umi_test_runtime_query_validate(const UmiTestRuntimeQuery *value);
UmiStatus umi_test_runtime_query_set_name(UmiTestRuntimeQuery *value, const char *name);
UmiStatus umi_test_runtime_query_set_detail(UmiTestRuntimeQuery *value, const char *detail);
UmiStatus umi_test_runtime_query_set_offset(UmiTestRuntimeQuery *value, uint64_t number);
UmiStatus umi_test_runtime_query_set_limit(UmiTestRuntimeQuery *value, uint64_t number);
UmiStatus umi_test_runtime_query_touch(UmiTestRuntimeQuery *value, uint64_t updated_at_ms);
bool umi_test_runtime_query_same_identity(const UmiTestRuntimeQuery *left, const UmiTestRuntimeQuery *right);

#ifdef __cplusplus
}
#endif
#endif
