/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/run_menu.c
 *
 * PURPOSE:
 *   Define the Framework-owned Run Menu contribution set. Applications render
 *   these records and bind command identifiers through the shared command
 *   registry rather than maintaining private menu or panel definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/profiles/run_menu.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.run-menu.root",
        .title = "Run",
        .description = "Top-level Run menu.",
        .command_id = "",
        .component_id = "",
        .group_id = "menu-bar",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 600,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.run-menu.run",
        .title = "Run",
        .description = "Run",
        .command_id = "run.start",
        .component_id = "",
        .group_id = "run",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.run-menu.run-with",
        .title = "Run With...",
        .description = "Run With...",
        .command_id = "run.select-configuration",
        .component_id = "",
        .group_id = "run",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 110,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.run-menu.stop",
        .title = "Stop",
        .description = "Stop",
        .command_id = "run.stop",
        .component_id = "",
        .group_id = "run",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 120,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.run-menu.restart",
        .title = "Restart",
        .description = "Restart",
        .command_id = "run.restart",
        .component_id = "",
        .group_id = "run",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 130,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.run-menu.debug",
        .title = "Debug",
        .description = "Debug",
        .command_id = "debug.start",
        .component_id = "",
        .group_id = "debug",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 200,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.run-menu.attach",
        .title = "Attach to Process",
        .description = "Attach to Process",
        .command_id = "debug.attach",
        .component_id = "",
        .group_id = "debug",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 210,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.run-menu.breakpoints",
        .title = "Breakpoints",
        .description = "Breakpoints",
        .command_id = "debug.breakpoints",
        .component_id = "",
        .group_id = "debug",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 220,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_run_menu(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.run-menu",
        .title = "Run Menu",
        .description = "Reusable Framework-owned Run Menu contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
