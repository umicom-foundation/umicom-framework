/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_navigation.c
 *
 * PURPOSE:
 *   Implement the test diagnostic navigation behavior for
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
#include "umicom/diagnostic_ui/navigation.h"
int main(void)
{
    UmiDiagnosticModel *model = NULL;
    UmiDiagnosticSnapshot item;
    UmiDiagnosticFilter filter;
    UmiDiagnosticNavigation navigation;
    assert(umi_diagnostic_model_create(NULL, &model) == UMI_STATUS_OK);
    assert(umi_diagnostic_snapshot_init(&item, "one", UMI_DIAGNOSTIC_ERROR,
        UMI_DIAGNOSTIC_KIND_GENERAL, "test", "one") == UMI_STATUS_OK);
    assert(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    assert(umi_diagnostic_snapshot_init(&item, "two", UMI_DIAGNOSTIC_ERROR,
        UMI_DIAGNOSTIC_KIND_GENERAL, "test", "two") == UMI_STATUS_OK);
    assert(umi_diagnostic_model_upsert(model, &item) == UMI_STATUS_OK);
    umi_diagnostic_filter_init(&filter);
    umi_diagnostic_navigation_init(&navigation);
    assert(umi_diagnostic_navigation_next(&navigation, model, &filter, &item) == UMI_STATUS_OK);
    assert(item.sequence == 1U);
    assert(umi_diagnostic_navigation_previous(&navigation, model, &filter, &item) == UMI_STATUS_OK);
    assert(item.sequence == 2U);
    umi_diagnostic_model_destroy(model);
    return 0;
}
