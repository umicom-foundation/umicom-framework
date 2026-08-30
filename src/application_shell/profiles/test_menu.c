/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/test_menu.c
 *
 * PURPOSE:
 *   Define the Framework-owned Test Menu contribution set. Applications render
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
#include "umicom/application_shell/profiles/test_menu.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.test-menu.root",
        .title = "Test",
        .description = "Top-level Test menu.",
        .command_id = "",
        .component_id = "",
        .group_id = "menu-bar",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 700,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.test-menu.discover",
        .title = "Discover Tests",
        .description = "Discover Tests",
        .command_id = "test.discover",
        .component_id = "",
        .group_id = "test",
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
        .contribution_id = "umicom.shell.test-menu.run-all",
        .title = "Run All Tests",
        .description = "Run All Tests",
        .command_id = "test.run-all",
        .component_id = "",
        .group_id = "test",
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
        .contribution_id = "umicom.shell.test-menu.run-selected",
        .title = "Run Selected",
        .description = "Run Selected",
        .command_id = "test.run-selected",
        .component_id = "",
        .group_id = "test",
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
        .contribution_id = "umicom.shell.test-menu.debug-selected",
        .title = "Debug Selected",
        .description = "Debug Selected",
        .command_id = "test.debug-selected",
        .component_id = "",
        .group_id = "test",
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
        .contribution_id = "umicom.shell.test-menu.rerun-failed",
        .title = "Rerun Failed",
        .description = "Rerun Failed",
        .command_id = "test.rerun-failed",
        .component_id = "",
        .group_id = "test",
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
        .contribution_id = "umicom.shell.test-menu.coverage",
        .title = "Run with Coverage",
        .description = "Run with Coverage",
        .command_id = "test.coverage-all",
        .component_id = "",
        .group_id = "coverage",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 300,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.test-menu.stop",
        .title = "Stop Tests",
        .description = "Stop Tests",
        .command_id = "test.stop",
        .component_id = "",
        .group_id = "test",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 400,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_test_menu(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.test-menu",
        .title = "Test Menu",
        .description = "Reusable Framework-owned Test Menu contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
