/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/layout_manager.c
 *
 * PURPOSE:
 *   Define the Framework-owned Layout Manager contribution set. Applications render
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
#include "umicom/application_shell/profiles/layout_manager.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.layout-manager.manager",
        .title = "Layout Manager",
        .description = "Layout Manager",
        .command_id = "layout.manager",
        .component_id = "umicom.shell.workspace-profiles",
        .group_id = "layout",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_DASHBOARD,
        .region = UMI_APPLICATION_SHELL_REGION_WINDOW,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED | UMI_APPLICATION_SHELL_MOVABLE | UMI_APPLICATION_SHELL_CLOSABLE,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.layout-manager.save",
        .title = "Save Layout",
        .description = "Save Layout",
        .command_id = "layout.save",
        .component_id = "",
        .group_id = "layout",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_WINDOW,
        .order = 110,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.layout-manager.restore",
        .title = "Restore Layout",
        .description = "Restore Layout",
        .command_id = "layout.restore",
        .component_id = "",
        .group_id = "layout",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_WINDOW,
        .order = 120,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.layout-manager.new-window",
        .title = "New Window",
        .description = "New Window",
        .command_id = "window.new",
        .component_id = "",
        .group_id = "layout",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_WINDOW,
        .order = 130,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_layout_manager(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.layout-manager",
        .title = "Layout Manager",
        .description = "Reusable Framework-owned Layout Manager contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
