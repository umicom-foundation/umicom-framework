/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_abi_compatibility.c
 *
 * PURPOSE:
 *   Protect the legacy suite-layout entry points and the resolver-aware entry
 *   points from accidentally sharing a C symbol or drifting out of sync with
 *   their public declarations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/umicom.h"
#include "umicom/application/suite_layout/profile_projection.h"
#include "umicom/application/suite_layout/workbench_bridge.h"

typedef UmiStatus (*LegacyProfileProject)(
    const UmiApplicationExperienceDefinition *,
    const UmiExperienceLayoutDefinition *,
    int32_t,
    UmiUiWorkspaceProfileSnapshot *);

typedef UmiStatus (*ResolvedProfileProject)(
    const UmiApplicationExperienceDefinition *,
    const UmiExperienceLayoutDefinition *,
    int32_t,
    UmiApplicationSuiteLayoutPaneResolver,
    void *,
    UmiUiWorkspaceProfileSnapshot *);

typedef UmiStatus (*LegacyWorkbenchRegister)(
    const UmiApplicationExperienceDefinition *,
    UmiUiWorkbench *,
    int);

typedef UmiStatus (*ResolvedWorkbenchRegister)(
    const UmiApplicationExperienceDefinition *,
    UmiUiWorkbench *,
    int,
    UmiApplicationSuiteLayoutPaneResolver,
    void *);

/*
 * Exercise resolve pane and return a clear result when the behaviour no longer matches its
 * contract.
 */
static const char *resolve_pane(
    const UmiExperiencePanelDefinition *panel,
    void *user_data)
{
    static char pane_id[UMI_UI_ID_CAPACITY];
    const char *prefix = (const char *)user_data;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (panel == NULL || panel->panel_id == NULL || prefix == NULL) return NULL;
    written = snprintf(pane_id, sizeof(pane_id), "%s%s", prefix,
                       panel->panel_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(pane_id)) return NULL;
    return pane_id;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    LegacyProfileProject legacy_profile =
        umi_application_suite_layout_profile_project;
    ResolvedProfileProject resolved_profile =
        umi_application_suite_layout_profile_project_resolved;
    LegacyWorkbenchRegister legacy_register =
        umi_application_suite_layout_register_workbench_profiles;
    ResolvedWorkbenchRegister resolved_register =
        umi_application_suite_layout_register_workbench_profiles_resolved;
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.studio");
    const UmiExperienceLayoutDefinition *layout;
    UmiUiWorkspaceProfileSnapshot profile;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    char profile_id[UMI_UI_ID_CAPACITY];
    int written;

    assert(experience != NULL);
    layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    assert(layout != NULL);

    assert(legacy_profile(experience, layout, 100, &profile) == UMI_STATUS_OK);
    assert(profile.pane_count > 0U);
    assert(strncmp(profile.panes[0].pane_id, "compat.", 7U) != 0);

    assert(resolved_profile(experience, layout, 101, resolve_pane,
                            (void *)"compat.", &profile) == UMI_STATUS_OK);
    assert(profile.pane_count > 0U);
    assert(strncmp(profile.panes[0].pane_id, "compat.", 7U) == 0);

    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("suite-layout.abi", commands, &workbench) ==
           UMI_STATUS_OK);
    assert(legacy_register(experience, workbench, 0) == UMI_STATUS_OK);

    written = snprintf(profile_id, sizeof(profile_id), "%s.%s",
                       experience->application_id,
                       experience->default_layout_id);
    assert(written >= 0 && (size_t)written < sizeof(profile_id));
    assert(umi_ui_workspace_profile_model_find(
               umi_ui_workbench_workspace_profiles(workbench), profile_id,
               &profile) == UMI_STATUS_OK);
    assert(profile.pane_count > 0U);
    assert(strncmp(profile.panes[0].pane_id, "compat.", 7U) != 0);

    assert(resolved_register(experience, workbench, 1, resolve_pane,
                             (void *)"compat.") == UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_find(
               umi_ui_workbench_workspace_profiles(workbench), profile_id,
               &profile) == UMI_STATUS_OK);
    assert(profile.pane_count > 0U);
    assert(strncmp(profile.panes[0].pane_id, "compat.", 7U) == 0);

    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return 0;
}
