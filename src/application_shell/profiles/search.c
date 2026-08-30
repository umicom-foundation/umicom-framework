/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/search.c
 *
 * PURPOSE:
 *   Define the Framework-owned Search contribution set. Applications render
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
#include "umicom/application_shell/profiles/search.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.search.search",
        .title = "Search",
        .description = "Search",
        .command_id = "",
        .component_id = "",
        .group_id = "search",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_VIEW,
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED | UMI_APPLICATION_SHELL_MOVABLE | UMI_APPLICATION_SHELL_CLOSABLE | UMI_APPLICATION_SHELL_CONTEXT_AWARE,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.search.replace",
        .title = "Replace",
        .description = "Replace",
        .command_id = "search.replace",
        .component_id = "",
        .group_id = "search",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 110,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.search.refresh",
        .title = "Refresh",
        .description = "Refresh",
        .command_id = "search.refresh",
        .component_id = "",
        .group_id = "search",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 120,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_search(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.search",
        .title = "Search",
        .description = "Reusable Framework-owned Search contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
