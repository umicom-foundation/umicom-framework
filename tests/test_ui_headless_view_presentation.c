/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_headless_view_presentation.c
 *
 * PURPOSE:
 *   Verify that the reference headless adapter renders a pane supplied through
 *   the same Framework view-factory/presentation path used by GTK4.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/runtime/command_registry.h"
#include "umicom/ui/application_shell.h"
#include "umicom/ui/headless.h"
#include "umicom/ui/workbench.h"

static UmiStatus create_demo_view(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiUiValue value;
    UmiStatus status;
    (void)user_data;

    status = umi_ui_view_model_create(
        view_id, "test.headless-view", UMI_UI_ROLE_PANE, out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ui_value_set_string(&value, "Headless View");
    if (status == UMI_STATUS_OK) {
        status = umi_ui_view_model_set_property(*out_view, "title", &value);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_value_set_boolean(&value, 1);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_view_model_set_property(*out_view, "ready", &value);
    }

    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiUiHeadlessAdapter *adapter = NULL;
    UmiUiHeadlessSnapshot render;
    UmiUiPaneSnapshot pane = {0};
    UmiUiViewFactoryDescriptor factory = {0};

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create(
               "test.workbench", commands, &workbench) == UMI_STATUS_OK);

    (void)snprintf(pane.pane_id, sizeof(pane.pane_id), "%s", "test.pane");
    (void)snprintf(pane.title, sizeof(pane.title), "%s", "Test Pane");
    (void)snprintf(pane.view_type, sizeof(pane.view_type), "%s",
                   "test.headless-view");
    pane.placement = UMI_UI_PLACEMENT_LEFT;
    pane.visible = 1;
    assert(umi_ui_pane_model_upsert(
               umi_ui_workbench_panes(workbench), &pane) == UMI_STATUS_OK);

    (void)snprintf(factory.view_type, sizeof(factory.view_type), "%s",
                   "test.headless-view");
    (void)snprintf(factory.provider_id, sizeof(factory.provider_id), "%s",
                   "org.umicom.framework.tests");
    factory.create = create_demo_view;
    assert(umi_ui_view_factory_register(
               umi_ui_workbench_view_factories(workbench), &factory) ==
           UMI_STATUS_OK);

    assert(umi_ui_application_shell_create(
               "test.application", "Headless Test", workbench, &shell) ==
           UMI_STATUS_OK);
    assert(umi_ui_headless_adapter_create(&adapter) == UMI_STATUS_OK);
    assert(umi_ui_headless_adapter_present(adapter, shell) == UMI_STATUS_OK);
    assert(umi_ui_headless_adapter_snapshot(adapter, &render) ==
           UMI_STATUS_OK);

    assert(strstr(render.text, "[view:test.pane]") != NULL);
    assert(strstr(render.text, "title=Headless View") != NULL);
    assert(strstr(render.text, "ready=true") != NULL);

    umi_ui_headless_adapter_destroy(adapter);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return 0;
}
