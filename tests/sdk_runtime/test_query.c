/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_query.c
 *
 * PURPOSE:
 *   Verify the query contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/query.h"
int main(void)
{
    UmiSdkRuntimeQuery value; UmiSdkRuntimeQuery same; uint64_t revision;
    umi_sdk_runtime_query_init(&value, "sdk-runtime.query");
    assert(umi_sdk_runtime_query_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_query_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_query_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_query_set_offset(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_query_set_limit(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_query_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.offset == 3U && value.limit == 5U);
    umi_sdk_runtime_query_init(&same, "sdk-runtime.query");
    assert(umi_sdk_runtime_query_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
