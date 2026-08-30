/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_ui_views.c
 *
 * PURPOSE:
 *   Verify reusable architecture views expose live portfolio and audit data.
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

#include "umicom/application_ui/application_ui.h"

static int64_t integer_property(UmiUiViewModel *view, const char *key)
{
    UmiUiValue value;
    assert(umi_ui_view_model_get_property(view, key, &value) == UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_INTEGER);
    return value.integer_value;
}

int main(void)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    assert(umi_application_ui_portfolio_view_create(
        "test.portfolio", "org.umicom.trader", &view) == UMI_STATUS_OK);
    assert(integer_property(view, "applications.total") >= 22);
    assert(umi_ui_view_model_get_property(view, "selected.id", &value) ==
           UMI_STATUS_OK);
    assert(strcmp(value.string_value, "org.umicom.trader") == 0);
    umi_ui_view_model_destroy(view);

    assert(umi_application_ui_boundary_audit_view_create(
        "test.audit", &view) == UMI_STATUS_OK);
    assert(integer_property(view, "audit.errors") == 0);
    umi_ui_view_model_destroy(view);

    assert(umi_application_ui_gtk4_coverage_view_create(
        "test.gtk4", &view) == UMI_STATUS_OK);
    assert(integer_property(view, "gtk4.widgets") >= 90);
    umi_ui_view_model_destroy(view);
    return 0;
}
