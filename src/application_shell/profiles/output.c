/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/output.c
 *
 * PURPOSE:
 *   Define the Framework-owned Output contribution set. Applications render
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
#include "umicom/application_shell/profiles/output.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.output.output",
        .title = "Output",
        .description = "Output",
        .command_id = "",
        .component_id = "",
        .group_id = "output",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED | UMI_APPLICATION_SHELL_MOVABLE | UMI_APPLICATION_SHELL_CLOSABLE,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.output.clear",
        .title = "Clear Output",
        .description = "Clear Output",
        .command_id = "output.clear",
        .component_id = "",
        .group_id = "output",
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
        .contribution_id = "umicom.shell.output.follow",
        .title = "Follow Output",
        .description = "Follow Output",
        .command_id = "output.follow",
        .component_id = "",
        .group_id = "output",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 120,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED | UMI_APPLICATION_SHELL_CHECKABLE,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_output(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.output",
        .title = "Output",
        .description = "Reusable Framework-owned Output contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
