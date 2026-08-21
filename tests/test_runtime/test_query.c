/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_query.c
 *
 * PURPOSE:
 *   Verify the query contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/query.h"

int main(void)
{
    UmiTestRuntimeQuery value;
    UmiTestRuntimeQuery same;
    uint64_t revision;
    umi_test_runtime_query_init(&value, "test-runtime.query");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_query_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_query_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_query_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_query_set_offset(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_query_set_limit(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_query_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.offset == 7U);
    assert(value.limit == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_query_init(&same, "test-runtime.query");
    assert(umi_test_runtime_query_same_identity(&value, &same));
    assert(umi_test_runtime_query_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
