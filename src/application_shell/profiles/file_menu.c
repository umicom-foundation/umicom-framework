/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/profiles/file_menu.c
 *
 * PURPOSE:
 *   Define the Framework-owned File Menu contribution set. Applications render
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
#include "umicom/application_shell/profiles/file_menu.h"

/* Task groups are semantic presentation metadata. Native hosts insert a
 * separator between visible groups without owning a second File inventory.
 * The former common group values remain disabled beside their replacements. */
static const UmiApplicationShellContribution CONTRIBUTIONS[] = {
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.save-as",
        .title = "Save As...",
        .description = "Save this document under a chosen path and continue editing that file.",
        .command_id = "file.save-as",
        .group_id = "file.save",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 205,
        .flags = UMI_APPLICATION_SHELL_VISIBLE,
        .revision = 1U
    },
    /* Separate copies preserve the source document; hosts supply admission. */
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.save-copy",
        .title = "Save Copy...",
        .description = "Write the captured draft to a new file and keep the source unchanged.",
        .command_id = "file.save-copy",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.save",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 245,
        .flags = UMI_APPLICATION_SHELL_VISIBLE,
        .revision = 1U
    },
    /* Shared captions route to a host-owned opening queue; enablement is queried. */
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.cancel-opening",
        .title = "Cancel Opening Files",
        .description = "Stop pending opens without closing documents that already opened.",
        .command_id = "file.open.cancel",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.open",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 145,
        /* A host enables this only when its opening queue has the required state. */
        .flags = UMI_APPLICATION_SHELL_VISIBLE,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.opening-results",
        .title = "Opening Results",
        .description = "Review each outcome from the latest completed file selection.",
        .command_id = "file.open.results",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.open",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 146,
        /* A host enables this only when its opening queue has the required state. */
        .flags = UMI_APPLICATION_SHELL_VISIBLE,
        .revision = 1U
    },
    /* Hosts bind these to the document coordinator, then query its history. */
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.reopen-closed",
        .title = "Reopen Closed Document",
        .description = "Open the latest saved path again without restoring discarded edits.",
        .command_id = "file.reopen-closed",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.close",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 340,
        .flags = UMI_APPLICATION_SHELL_VISIBLE,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.forget-closed",
        .title = "Forget Last Closed Document",
        .description = "Skip the latest closed path without deleting its file.",
        .command_id = "file.forget-closed",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.close",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 350,
        .flags = UMI_APPLICATION_SHELL_VISIBLE,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.close-all",
        .title = "Close All Source Documents",
        .description = "Review and close the source documents open when this action starts.",
        .command_id = "file.close-all",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.close",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 310,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .revision = 1U
    },

    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.close-others",
        .title = "Close Other Source Documents",
        .description = "Keep the current source and review the other captured documents.",
        .command_id = "file.close-others",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.close",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 320,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .revision = 1U
    },

    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.cancel-close-all",
        .title = "Cancel Closing Documents",
        .description = "Stop remaining closes; previously saved and closed documents stay completed.",
        .command_id = "file.close-all.cancel",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.close",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 330,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .revision = 1U
    },

    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.new-file",
        .title = "New File",
        .description = "New File",
        .command_id = "file.new",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.open",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 105,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.root",
        .title = "File",
        .description = "Top-level File menu.",
        .command_id = "",
        .component_id = "",
        .group_id = "menu-bar",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 100,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.new-project",
        .title = "New Project",
        .description = "Create a project from a Framework application preset.",
        .command_id = "project.new",
        .component_id = "",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.open",
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
        .contribution_id = "umicom.shell.file-menu.open",
        .title = "Open...",
        .description = "Open...",
        .command_id = "file.open",
        .component_id = "",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.open",
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
        .contribution_id = "umicom.shell.file-menu.open-folder",
        .title = "Open Folder...",
        .description = "Open Folder...",
        .command_id = "workspace.open-folder",
        .component_id = "",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.open",
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
        .contribution_id = "umicom.shell.file-menu.save",
        .title = "Save",
        .description = "Save",
        .command_id = "file.save",
        .component_id = "",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.save",
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
        .contribution_id = "umicom.shell.file-menu.save-all",
        .title = "Save All",
        .description = "Save All",
        .command_id = "file.save-all",
        .component_id = "",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.save",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 210,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    /* Saved-file reload is a reviewed editor action, separate from saving. */
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.reload",
        .title = "Reload from Disk...",
        .description = "Compare the saved file with the current draft before replacing it.",
        .command_id = "file.reload",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.save",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 218,
        .flags = UMI_APPLICATION_SHELL_VISIBLE,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.cancel-save-all",
        .title = "Cancel Save All",
        .description = "Stop the remaining saves; keep saved files and unsaved drafts.",
        .command_id = "file.save-all.cancel",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.save",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 215,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.close",
        .title = "Close Editor",
        .description = "Close Editor",
        .command_id = "file.close",
        .component_id = "",
#if 0
        .group_id = "file",
#endif
        .group_id = "file.close",
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
        .contribution_id = "umicom.shell.file-menu.install",
        .title = "Install",
        .description = "Install",
        .command_id = "build.install",
        .component_id = "",
        .group_id = "delivery",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 500,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.package",
        .title = "Package",
        .description = "Package",
        .command_id = "build.package",
        .component_id = "",
        .group_id = "delivery",
        .icon_id = "",
        .context_key = "",
        .role = UMI_APPLICATION_SHELL_ROLE_MENU_ITEM,
        .region = UMI_APPLICATION_SHELL_REGION_MENU_BAR,
        .order = 510,
        .flags = UMI_APPLICATION_SHELL_VISIBLE | UMI_APPLICATION_SHELL_ENABLED,
        .badge_count = 0U,
        .revision = 1U
    },
    {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellContribution),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .contribution_id = "umicom.shell.file-menu.exit",
        .title = "Exit",
        .description = "Exit",
        .command_id = "application.exit",
        .component_id = "",
        .group_id = "application",
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

/*
 * Provide the application shell profile file menu operation used by this module and its
 * client applications.
 */
const UmiApplicationShellProfileDefinition *umi_application_shell_profile_file_menu(void)
{
    static const UmiApplicationShellProfileDefinition profile = {
        .structure_size = (uint32_t)sizeof(UmiApplicationShellProfileDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .profile_id = "umicom.shell.profile.file-menu",
        .title = "File Menu",
        .description = "Reusable Framework-owned File Menu contributions for thin applications.",
        .contributions = CONTRIBUTIONS,
        .contribution_count = sizeof(CONTRIBUTIONS) / sizeof(CONTRIBUTIONS[0])
    };

    return &profile;
}
