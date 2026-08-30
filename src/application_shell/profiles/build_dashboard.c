/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/build_dashboard.c
 *
 * PURPOSE:
 *   Define the Framework-owned Build Dashboard contribution set. Applications render
 *   these records and bind command identifiers through the shared command
 *   registry rather than maintaining private menu or panel definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/profiles/build_dashboard.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.build-dashboard.dashboard",
        .title = "Build Dashboard",
        .description = "Build Dashboard",
        .command_id = "",
        .component_id = "umicom.development.build",
        .group_id = "build",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_DASHBOARD,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED | UMI_APPLICATION_SHELL_MOVABLE | UMI_APPLICATION_SHELL_CLOSABLE | UMI_APPLICATION_SHELL_CONTEXT_AWARE,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.build-dashboard.configure",
        .title = "Configure",
        .description = "Configure",
        .command_id = "build.configure",
        .component_id = "",
        .group_id = "build",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 110,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.build-dashboard.build",
        .title = "Build",
        .description = "Build",
        .command_id = "build.build",
        .component_id = "",
        .group_id = "build",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 120,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.build-dashboard.cancel",
        .title = "Cancel",
        .description = "Cancel",
        .command_id = "build.cancel",
        .component_id = "",
        .group_id = "build",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 130,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_build_dashboard(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.build-dashboard",
        .title = "Build Dashboard",
        .description = "Reusable Framework-owned Build Dashboard contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
