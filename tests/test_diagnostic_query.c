/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_query.c
 *
 * PURPOSE:
 *   Implement the test diagnostic query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/diagnostics/query.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDiagnosticModel *model = NULL;
    UmiDiagnosticSnapshot item;
    UmiDiagnosticQuery query;
    UmiDiagnosticQueryResult result;
    assert(umi_diagnostic_model_create(NULL, &model) == UMI_STATUS_OK);
    assert(umi_diagnostic_snapshot_init(&item, "info", UMI_DIAGNOSTIC_INFO,
        UMI_DIAGNOSTIC_KIND_GENERAL, "test", "info") == UMI_STATUS_OK);
    assert(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    assert(umi_diagnostic_snapshot_init(&item, "fatal", UMI_DIAGNOSTIC_FATAL,
        UMI_DIAGNOSTIC_KIND_RUNTIME, "test", "fatal") == UMI_STATUS_OK);
    assert(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    umi_diagnostic_query_init(&query);
    query.sort = UMI_DIAGNOSTIC_SORT_SEVERITY_DESCENDING;
    assert(umi_diagnostic_query_execute(model, &query, &result) == UMI_STATUS_OK);
    assert(result.count == 2U && result.items[0].severity == UMI_DIAGNOSTIC_FATAL);
    umi_diagnostic_model_destroy(model);
    return 0;
}
