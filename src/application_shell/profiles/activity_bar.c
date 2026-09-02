/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/activity_bar.c
 *
 * PURPOSE:
 *   Define the Framework-owned Activity Bar contribution set. Applications render
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
#include "umicom/application_shell/profiles/activity_bar.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.activity-bar.explorer",
        .title = "Explorer",
        .description = "Explorer",
        .command_id = "view.explorer",
        .component_id = "umicom.development.explorer",
        .group_id = "navigation",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.activity-bar.search",
        .title = "Search",
        .description = "Search",
        .command_id = "view.search",
        .component_id = "",
        .group_id = "navigation",
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
        .contribution_id = "umicom.shell.activity-bar.source-control",
        .title = "Source Control",
        .description = "Source Control",
        .command_id = "view.source-control",
        .component_id = "umicom.development.source-control",
        .group_id = "navigation",
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
        .contribution_id = "umicom.shell.activity-bar.run-debug",
        .title = "Run and Debug",
        .description = "Run and Debug",
        .command_id = "view.debug",
        .component_id = "umicom.development.debug",
        .group_id = "navigation",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR,
        .order = 130,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.activity-bar.testing",
        .title = "Testing",
        .description = "Testing",
        .command_id = "view.test-explorer",
        .component_id = "umicom.development.testing",
        .group_id = "navigation",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR,
        .order = 140,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.activity-bar.extensions",
        .title = "Extensions",
        .description = "Extensions",
        .command_id = "extensions.open",
        .component_id = "",
        .group_id = "navigation",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR,
        .order = 150,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.activity-bar.ai",
        .title = "AI Assistant",
        .description = "AI Assistant",
        .command_id = "ai.assistant.open",
        .component_id = "umicom.ai.chat",
        .group_id = "navigation",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR,
        .order = 160,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

/*
 * Provide the application shell profile activity bar operation used by this module and its
 * client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_activity_bar(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.activity-bar",
        .title = "Activity Bar",
        .description = "Reusable Framework-owned Activity Bar contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
