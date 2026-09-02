/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_query.c
 *
 * PURPOSE:
 *   Verify query record initialization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    UmiWorkbenchContextHostQuery query;
    umi_workbench_context_host_query_init(
        &query, UMI_WORKBENCH_CONTEXT_HOST_QUERY_SNAPSHOT);
    assert(query.structure_size == sizeof(query));
    assert(query.kind == UMI_WORKBENCH_CONTEXT_HOST_QUERY_SNAPSHOT);
    return 0;
}
