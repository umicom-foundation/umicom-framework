/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_query.c
 *
 * PURPOSE:
 *   Verify query record initialization.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostQuery query;
    umi_workbench_context_host_query_init(
        &query, UMI_WORKBENCH_CONTEXT_HOST_QUERY_SNAPSHOT);
    assert(query.structure_size == sizeof(query));
    assert(query.kind == UMI_WORKBENCH_CONTEXT_HOST_QUERY_SNAPSHOT);
    return 0;
}
