/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_ui_v2.c
 *
 * PURPOSE:
 *   Verify toolkit-neutral Extension Centre view projections.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include "plugin_v2_fixture.h"

int main(void)
{
    UmiPluginManager *manager = NULL;
    UmiPluginManifest manifest = plugin_test_manifest("org.example", "Example");
    UmiPluginPolicyDecision decision;
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    assert(umi_plugin_manager_create(NULL, NULL, &manager) == UMI_STATUS_OK);
    assert(umi_permission_set_add(umi_plugin_host_grants(
        umi_plugin_manager_host(manager)), "*") == UMI_STATUS_OK);
    assert(umi_plugin_manager_register(manager, &manifest,
        "example.umicom-plugin", UMI_PLUGIN_TRUST_LOCAL, 100U, &decision) == UMI_STATUS_OK);
    assert(umi_plugin_ui_installed_view_create(
        "test.extensions", manager, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "extensions.installed", &value) == UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_INTEGER);
    assert(value.integer_value == 1);
    assert(umi_ui_view_model_get_property(
        view, UMI_UI_COMMAND_VIEW_ACTION_COUNT_KEY, &value) == UMI_STATUS_OK);
    assert(value.integer_value == 4);
    umi_ui_view_model_destroy(view);
    assert(umi_plugin_ui_audit_view_create(
        "test.audit", manager, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    umi_plugin_manager_destroy(manager);
    return 0;
}
