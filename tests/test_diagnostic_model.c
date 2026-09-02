/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_model.c
 *
 * PURPOSE:
 *   Implement the test diagnostic model behavior for
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
#include <string.h>
#include "umicom/diagnostics/model.h"
/*
 * Exercise make item and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiDiagnosticSnapshot make_item(const char *id, UmiDiagnosticSeverity severity)
{
    UmiDiagnosticSnapshot item;
    assert(umi_diagnostic_snapshot_init(&item, id, severity,
        UMI_DIAGNOSTIC_KIND_GENERAL, "test", id) == UMI_STATUS_OK);
    return item;
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDiagnosticModelConfig config = {2U};
    UmiDiagnosticModel *model = NULL;
    UmiDiagnosticModelSummary summary;
    UmiDiagnosticSnapshot item = make_item("one", UMI_DIAGNOSTIC_WARNING);
    assert(umi_diagnostic_model_create(&config, &model) == UMI_STATUS_OK);
    assert(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    item = make_item("two", UMI_DIAGNOSTIC_ERROR);
    assert(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    item = make_item("three", UMI_DIAGNOSTIC_FATAL);
    assert(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    assert(umi_diagnostic_model_find(model, "one", &item) == UMI_STATUS_NOT_FOUND);
    assert(umi_diagnostic_model_resolve(model, "two", 1) == UMI_STATUS_OK);
    assert(umi_diagnostic_model_summary(model, &summary) == UMI_STATUS_OK);
    assert(summary.retained_count == 2U && summary.overwritten_count == 1U);
    assert(summary.resolved_count == 1U);
    umi_diagnostic_model_destroy(model);
    return 0;
}
