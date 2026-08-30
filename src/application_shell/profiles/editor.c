/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/editor.c
 *
 * PURPOSE:
 *   Define the Framework-owned Editor contribution set. Applications render
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
#include "umicom/application_shell/profiles/editor.h"

static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.editor.code-editor",
        .title = "Code Editor",
        .description = "Code Editor",
        .command_id = "",
        .component_id = "umicom.development.editor",
        .group_id = "editor",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_EDITOR,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED | UMI_APPLICATION_SHELL_MULTI_INSTANCE | UMI_APPLICATION_SHELL_CONTEXT_AWARE,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.editor.split-right",
        .title = "Split Right",
        .description = "Split Right",
        .command_id = "window.split-right",
        .component_id = "",
        .group_id = "editor",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 200,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.editor.split-down",
        .title = "Split Down",
        .description = "Split Down",
        .command_id = "window.split-down",
        .component_id = "",
        .group_id = "editor",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 210,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.editor.breadcrumbs",
        .title = "Breadcrumbs",
        .description = "Breadcrumbs",
        .command_id = "",
        .component_id = "",
        .group_id = "editor",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 300,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    }
};

const UmiApplicationShellProfileDefinition *umi_application_shell_profile_editor(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.editor",
        .title = "Editor",
        .description = "Reusable Framework-owned Editor contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
