/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/start_center.c
 *
 * PURPOSE:
 *   Define the Framework-owned Start Center contribution set. Applications render
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
#include "umicom/application_shell/profiles/start_center.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.start-center.welcome",
        .title = "Welcome",
        .description = "Welcome",
        .command_id = "",
        .component_id = "",
        .group_id = "start",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_VIEW,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.start-center.recent",
        .title = "Recent Projects",
        .description = "Recent Projects",
        .command_id = "",
        .component_id = "",
        .group_id = "start",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_VIEW,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 110,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.start-center.new-project",
        .title = "New Project",
        .description = "New Project",
        .command_id = "project.new",
        .component_id = "",
        .group_id = "start",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_WIZARD,
        .region = UMI_APPLICATION_SHELL_REGION_OVERLAY,
        .order = 120,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.start-center.clone",
        .title = "Clone Repository",
        .description = "Clone Repository",
        .command_id = "repository.clone",
        .component_id = "",
        .group_id = "start",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_WIZARD,
        .region = UMI_APPLICATION_SHELL_REGION_OVERLAY,
        .order = 130,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

/*
 * Provide the application shell profile start center operation used by this module and its
 * client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_start_center(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.start-center",
        .title = "Start Center",
        .description = "Reusable Framework-owned Start Center contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
