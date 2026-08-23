/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/status_bar.c
 *
 * PURPOSE:
 *   Define the Framework-owned Status Bar contribution set. Applications render
 *   these records and bind command identifiers through the shared command
 *   registry rather than maintaining private menu or panel definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/profiles/status_bar.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.status-bar.workspace",
        .title = "Workspace",
        .description = "Workspace",
        .command_id = "",
        .component_id = "",
        .group_id = "left",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_STATUS_BAR,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.status-bar.branch",
        .title = "Git Branch",
        .description = "Git Branch",
        .command_id = "",
        .component_id = "",
        .group_id = "left",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_STATUS_BAR,
        .order = 110,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.status-bar.problems",
        .title = "Problems",
        .description = "Problems",
        .command_id = "view.problems",
        .component_id = "",
        .group_id = "left",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_STATUS_BAR,
        .order = 120,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.status-bar.build",
        .title = "Build Status",
        .description = "Build Status",
        .command_id = "view.build-dashboard",
        .component_id = "umicom.development.build",
        .group_id = "left",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_STATUS_BAR,
        .order = 130,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.status-bar.language",
        .title = "Language Mode",
        .description = "Language Mode",
        .command_id = "",
        .component_id = "",
        .group_id = "right",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_STATUS_BAR,
        .order = 700,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.status-bar.encoding",
        .title = "Encoding",
        .description = "Encoding",
        .command_id = "",
        .component_id = "",
        .group_id = "right",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_STATUS_BAR,
        .order = 710,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.status-bar.line-column",
        .title = "Line / Column",
        .description = "Line / Column",
        .command_id = "",
        .component_id = "",
        .group_id = "right",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_STATUS_BAR,
        .order = 720,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_status_bar(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.status-bar",
        .title = "Status Bar",
        .description = "Reusable Framework-owned Status Bar contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
