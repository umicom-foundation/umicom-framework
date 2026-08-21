/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_templates_registries.c
 *
 * PURPOSE:
 *   Verify reusable Framework templates, panel contributions and perspectives.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

int main(void)
{
    UmiWorkbenchLayoutTemplateRegistry *templates =
        (UmiWorkbenchLayoutTemplateRegistry *)calloc(1U, sizeof(*templates));
    UmiWorkbenchPanelRegistry *panels =
        (UmiWorkbenchPanelRegistry *)calloc(1U, sizeof(*panels));
    UmiWorkbenchPerspectiveRegistry *perspectives =
        (UmiWorkbenchPerspectiveRegistry *)calloc(1U, sizeof(*perspectives));
    UmiWorkbenchLayoutDocument *cloned =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*cloned));
    UmiWorkbenchPanelDefinition panel;
    UmiWorkbenchPerspectiveDefinition perspective;
    UmiWorkbenchLayoutIdentity identity =
        test_layout_identity("layout.from-template");

    TEST_REQUIRE(
        templates != NULL && panels != NULL && perspectives != NULL &&
        cloned != NULL,
        "Registry test state must be allocated");
    umi_workbench_layout_template_registry_init(templates);
    TEST_STATUS_OK(umi_workbench_layout_template_registry_seed_framework(
        templates));
    TEST_REQUIRE(templates->count >= 4U, "Framework must seed useful layouts");
    TEST_REQUIRE(
        umi_workbench_layout_template_registry_find(
            templates, "framework.development") != NULL,
        "Development template must be discoverable");
    TEST_STATUS_OK(umi_workbench_layout_template_registry_clone(
        templates,
        "framework.development",
        &identity,
        "Developer Layout",
        cloned));
    TEST_REQUIRE(
        strcmp(cloned->identity.layout_id, "layout.from-template") == 0,
        "Template clone must receive a new stable identity");
    TEST_REQUIRE(
        umi_workbench_layout_document_has_flag(
            cloned, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY),
        "Template clone must be an unsaved working document");

    umi_workbench_panel_registry_init(panels);
    (void)memset(&panel, 0, sizeof(panel));
    panel.structure_size = sizeof(panel);
    test_copy_text(panel.panel_id, sizeof(panel.panel_id), "panel.problems");
    test_copy_text(
        panel.display_name,
        sizeof(panel.display_name),
        "Problems");
    test_copy_text(
        panel.owner_application_id,
        sizeof(panel.owner_application_id),
        "org.umicom.studio");
    test_copy_text(panel.category, sizeof(panel.category), "diagnostics");
    panel.default_region = UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM;
    panel.flags = UMI_WORKBENCH_PANEL_SINGLETON |
                  UMI_WORKBENCH_PANEL_CLOSABLE |
                  UMI_WORKBENCH_PANEL_MOVABLE;
    panel.revision = 1U;
    TEST_STATUS_OK(umi_workbench_panel_registry_add(panels, &panel));
    TEST_REQUIRE(
        umi_workbench_panel_registry_count_owner(
            panels, "org.umicom.studio") == 1U,
        "Panel registry must count application contributions");

    umi_workbench_perspective_registry_init(perspectives);
    (void)memset(&perspective, 0, sizeof(perspective));
    perspective.structure_size = sizeof(perspective);
    test_copy_text(
        perspective.perspective_id,
        sizeof(perspective.perspective_id),
        "perspective.debug");
    test_copy_text(
        perspective.display_name,
        sizeof(perspective.display_name),
        "Debug");
    test_copy_text(
        perspective.owner_application_id,
        sizeof(perspective.owner_application_id),
        "org.umicom.studio");
    test_copy_text(
        perspective.default_layout_id,
        sizeof(perspective.default_layout_id),
        "framework.development");
    perspective.default_for_application = true;
    perspective.revision = 1U;
    TEST_STATUS_OK(umi_workbench_perspective_definition_add_panel(
        &perspective, "panel.problems"));
    TEST_STATUS_OK(umi_workbench_perspective_definition_add_command(
        &perspective, "debug.start"));
    TEST_STATUS_OK(umi_workbench_perspective_registry_add(
        perspectives, &perspective));
    TEST_REQUIRE(
        umi_workbench_perspective_registry_default_for_application(
            perspectives, "org.umicom.studio") != NULL,
        "Default perspective must be resolved by application identity");

    free(cloned);
    free(perspectives);
    free(panels);
    free(templates);
    return 0;
}
