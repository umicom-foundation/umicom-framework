/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/edit_menu.c
 *
 * PURPOSE:
 *   Define the Framework-owned Edit Menu contribution set. Applications render
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
#include "umicom/application_shell/profiles/edit_menu.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.edit-menu.root",
        .title = "Edit",
        .description = "Top-level Edit menu.",
        .command_id = "",
        .component_id = "",
        .group_id = "menu-bar",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 200,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.edit-menu.undo",
        .title = "Undo",
        .description = "Undo",
        .command_id = "edit.undo",
        .component_id = "",
        .group_id = "history",
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
        .contribution_id = "umicom.shell.edit-menu.redo",
        .title = "Redo",
        .description = "Redo",
        .command_id = "edit.redo",
        .component_id = "",
        .group_id = "history",
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
        .contribution_id = "umicom.shell.edit-menu.cut",
        .title = "Cut",
        .description = "Cut",
        .command_id = "edit.cut",
        .component_id = "",
        .group_id = "clipboard",
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
        .contribution_id = "umicom.shell.edit-menu.copy",
        .title = "Copy",
        .description = "Copy",
        .command_id = "edit.copy",
        .component_id = "",
        .group_id = "clipboard",
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
        .contribution_id = "umicom.shell.edit-menu.paste",
        .title = "Paste",
        .description = "Paste",
        .command_id = "edit.paste",
        .component_id = "",
        .group_id = "clipboard",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 220,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.edit-menu.find",
        .title = "Find",
        .description = "Find",
        .command_id = "search.find",
        .component_id = "",
        .group_id = "search",
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
        .contribution_id = "umicom.shell.edit-menu.replace",
        .title = "Replace",
        .description = "Replace",
        .command_id = "search.replace",
        .component_id = "",
        .group_id = "search",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 310,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.edit-menu.find-files",
        .title = "Find in Files",
        .description = "Find in Files",
        .command_id = "search.workspace",
        .component_id = "",
        .group_id = "search",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 320,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_edit_menu(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.edit-menu",
        .title = "Edit Menu",
        .description = "Reusable Framework-owned Edit Menu contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
