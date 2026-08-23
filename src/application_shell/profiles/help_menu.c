/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/help_menu.c
 *
 * PURPOSE:
 *   Define the Framework-owned Help Menu contribution set. Applications render
 *   these records and bind command identifiers through the shared command
 *   registry rather than maintaining private menu or panel definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/profiles/help_menu.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.help-menu.root",
        .title = "Help",
        .description = "Top-level Help menu.",
        .command_id = "",
        .component_id = "",
        .group_id = "menu-bar",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 1100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.help-menu.welcome",
        .title = "Welcome",
        .description = "Welcome",
        .command_id = "help.welcome",
        .component_id = "",
        .group_id = "help",
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
        .contribution_id = "umicom.shell.help-menu.documentation",
        .title = "Documentation",
        .description = "Documentation",
        .command_id = "help.documentation",
        .component_id = "",
        .group_id = "help",
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
        .contribution_id = "umicom.shell.help-menu.ai-teacher",
        .title = "AI Teacher",
        .description = "AI Teacher",
        .command_id = "help.ai-teacher",
        .component_id = "",
        .group_id = "learning",
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
        .contribution_id = "umicom.shell.help-menu.shortcuts",
        .title = "Keyboard Shortcuts",
        .description = "Keyboard Shortcuts",
        .command_id = "help.shortcuts",
        .component_id = "",
        .group_id = "learning",
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
        .contribution_id = "umicom.shell.help-menu.about",
        .title = "About Umicom",
        .description = "About Umicom",
        .command_id = "help.about",
        .component_id = "",
        .group_id = "help",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 900,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_help_menu(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.help-menu",
        .title = "Help Menu",
        .description = "Reusable Framework-owned Help Menu contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
