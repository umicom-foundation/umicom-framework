/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/types.c
 *
 * PURPOSE:
 *   Implement reusable application-shell contribution initialisation,
 *   validation and stable diagnostic text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/types.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Initialise application shell contribution from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_shell_contribution_init(
    UmiApplicationShellContribution *contribution,
    const char *contribution_id,
    const char *title,
    UmiApplicationShellRole role,
    UmiApplicationShellRegion region)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Check that application shell contribution satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_shell_contribution_validate(
    const UmiApplicationShellContribution *contribution)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Apply this branch only when its contract condition is satisfied. */
    if ((contribution->role == UMI_APPLICATION_SHELL_ROLE_MENU_ITEM ||
         contribution->role == UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM ||
         contribution->role == UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM) &&
        contribution->command_id[0] == '\0' &&
        contribution->component_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the application shell role text operation used by this module and its client
 * applications.
 */
const char *umi_application_shell_role_text(UmiApplicationShellRole role)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the application shell region text operation used by this module and its client
 * applications.
 */
const char *umi_application_shell_region_text(UmiApplicationShellRegion region)
{
    /* Select the behaviour associated with the requested command or state value. */
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
