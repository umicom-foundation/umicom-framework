/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/types.c
 *
 * PURPOSE:
 *   Implement reusable application-shell contribution initialisation,
 *   validation and stable diagnostic text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/types.h"

#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";

    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

void umi_application_shell_contribution_init(
    UmiApplicationShellContribution *contribution,
    const char *contribution_id,
    const char *title,
    UmiApplicationShellRole role,
    UmiApplicationShellRegion region)
{
    if (contribution == NULL) return;

    (void)memset(contribution, 0, sizeof(*contribution));
    contribution->structure_size = (uint32_t)sizeof(*contribution);
    contribution->api_version = UMI_APPLICATION_SHELL_API_VERSION;
    copy_text(contribution->contribution_id,
              sizeof(contribution->contribution_id),
              contribution_id);
    copy_text(contribution->title,
              sizeof(contribution->title),
              title);
    contribution->role = role;
    contribution->region = region;
    contribution->flags =
        UMI_APPLICATION_SHELL_VISIBLE |
        UMI_APPLICATION_SHELL_ENABLED;
    contribution->revision = 1U;
}

UmiStatus umi_application_shell_contribution_validate(
    const UmiApplicationShellContribution *contribution)
{
    if (contribution == NULL ||
        contribution->structure_size != sizeof(*contribution) ||
        contribution->api_version != UMI_APPLICATION_SHELL_API_VERSION ||
        contribution->contribution_id[0] == '\0' ||
        contribution->title[0] == '\0' ||
        contribution->role < UMI_APPLICATION_SHELL_ROLE_MENU ||
        contribution->role > UMI_APPLICATION_SHELL_ROLE_SERVICE_SURFACE ||
        contribution->region < UMI_APPLICATION_SHELL_REGION_NONE ||
        contribution->region > UMI_APPLICATION_SHELL_REGION_WINDOW) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if ((contribution->role == UMI_APPLICATION_SHELL_ROLE_MENU_ITEM ||
         contribution->role == UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM ||
         contribution->role == UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM) &&
        contribution->command_id[0] == '\0' &&
        contribution->component_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

const char *umi_application_shell_role_text(UmiApplicationShellRole role)
{
    switch (role) {
        case UMI_APPLICATION_SHELL_ROLE_MENU: return "menu";
        case UMI_APPLICATION_SHELL_ROLE_MENU_ITEM: return "menu-item";
        case UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM: return "toolbar-item";
        case UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM: return "activity-item";
        case UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM: return "status-item";
        case UMI_APPLICATION_SHELL_ROLE_VIEW: return "view";
        case UMI_APPLICATION_SHELL_ROLE_PANEL: return "panel";
        case UMI_APPLICATION_SHELL_ROLE_EDITOR: return "editor";
        case UMI_APPLICATION_SHELL_ROLE_DASHBOARD: return "dashboard";
        case UMI_APPLICATION_SHELL_ROLE_WIZARD: return "wizard";
        case UMI_APPLICATION_SHELL_ROLE_PALETTE: return "palette";
        case UMI_APPLICATION_SHELL_ROLE_SERVICE_SURFACE:
            return "service-surface";
        default: return "unknown";
    }
}

const char *umi_application_shell_region_text(UmiApplicationShellRegion region)
{
    switch (region) {
        case UMI_APPLICATION_SHELL_REGION_NONE: return "none";
        case UMI_APPLICATION_SHELL_REGION_MENU_BAR: return "menu-bar";
        case UMI_APPLICATION_SHELL_REGION_TOOLBAR: return "toolbar";
        case UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR: return "activity-bar";
        case UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR:
            return "primary-sidebar";
        case UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR:
            return "secondary-sidebar";
        case UMI_APPLICATION_SHELL_REGION_EDITOR: return "editor";
        case UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL: return "bottom-panel";
        case UMI_APPLICATION_SHELL_REGION_STATUS_BAR: return "status-bar";
        case UMI_APPLICATION_SHELL_REGION_OVERLAY: return "overlay";
        case UMI_APPLICATION_SHELL_REGION_WINDOW: return "window";
        default: return "unknown";
    }
}
