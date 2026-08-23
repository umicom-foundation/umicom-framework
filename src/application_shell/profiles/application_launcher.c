/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/application_launcher.c
 *
 * PURPOSE:
 *   Define the Framework-owned Application Launcher contribution set. Applications render
 *   these records and bind command identifiers through the shared command
 *   registry rather than maintaining private menu or panel definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/profiles/application_launcher.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.application-launcher.launcher",
        .title = "Application Launcher",
        .description = "Application Launcher",
        .command_id = "",
        .component_id = "",
        .group_id = "applications",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_PALETTE,
        .region = UMI_APPLICATION_SHELL_REGION_OVERLAY,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.application-launcher.studio",
        .title = "Umicom Studio",
        .description = "Umicom Studio",
        .command_id = "application.launch.studio",
        .component_id = "",
        .group_id = "applications",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR,
        .order = 110,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.application-launcher.desk",
        .title = "Umicom Desk",
        .description = "Umicom Desk",
        .command_id = "application.launch.desk",
        .component_id = "",
        .group_id = "applications",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR,
        .order = 120,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.application-launcher.trader",
        .title = "Umicom Trader",
        .description = "Umicom Trader",
        .command_id = "application.launch.trader",
        .component_id = "",
        .group_id = "applications",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR,
        .order = 130,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_application_launcher(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.application-launcher",
        .title = "Application Launcher",
        .description = "Reusable Framework-owned Application Launcher contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
