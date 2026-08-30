/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/types.h
 *
 * PURPOSE:
 *   Define stable, toolkit-neutral contribution records for reusable application
 *   shells inspired by professional IDEs and multi-window trading workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_TYPES_H
#define UMICOM_APPLICATION_SHELL_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SHELL_API_VERSION 1U
#define UMI_APPLICATION_SHELL_ID_CAPACITY 192U
#define UMI_APPLICATION_SHELL_TITLE_CAPACITY 192U
#define UMI_APPLICATION_SHELL_TEXT_CAPACITY 512U
#define UMI_APPLICATION_SHELL_MAX_CONTRIBUTIONS 1024U
#define UMI_APPLICATION_SHELL_MAX_PROFILE_ITEMS 128U

typedef enum UmiApplicationShellRole {
    UMI_APPLICATION_SHELL_ROLE_MENU = 1,
    UMI_APPLICATION_SHELL_ROLE_MENU_ITEM = 2,
    UMI_APPLICATION_SHELL_ROLE_TOOLBAR_ITEM = 3,
    UMI_APPLICATION_SHELL_ROLE_ACTIVITY_ITEM = 4,
    UMI_APPLICATION_SHELL_ROLE_STATUS_ITEM = 5,
    UMI_APPLICATION_SHELL_ROLE_VIEW = 6,
    UMI_APPLICATION_SHELL_ROLE_PANEL = 7,
    UMI_APPLICATION_SHELL_ROLE_EDITOR = 8,
    UMI_APPLICATION_SHELL_ROLE_DASHBOARD = 9,
    UMI_APPLICATION_SHELL_ROLE_WIZARD = 10,
    UMI_APPLICATION_SHELL_ROLE_PALETTE = 11,
    UMI_APPLICATION_SHELL_ROLE_SERVICE_SURFACE = 12
} UmiApplicationShellRole;

typedef enum UmiApplicationShellRegion {
    UMI_APPLICATION_SHELL_REGION_NONE = 0,
    UMI_APPLICATION_SHELL_REGION_MENU_BAR = 1,
    UMI_APPLICATION_SHELL_REGION_TOOLBAR = 2,
    UMI_APPLICATION_SHELL_REGION_ACTIVITY_BAR = 3,
    UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR = 4,
    UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR = 5,
    UMI_APPLICATION_SHELL_REGION_EDITOR = 6,
    UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL = 7,
    UMI_APPLICATION_SHELL_REGION_STATUS_BAR = 8,
    UMI_APPLICATION_SHELL_REGION_OVERLAY = 9,
    UMI_APPLICATION_SHELL_REGION_WINDOW = 10
} UmiApplicationShellRegion;

typedef uint32_t UmiApplicationShellFlags;
enum {
    UMI_APPLICATION_SHELL_VISIBLE = 1U << 0,
    UMI_APPLICATION_SHELL_ENABLED = 1U << 1,
    UMI_APPLICATION_SHELL_CHECKABLE = 1U << 2,
    UMI_APPLICATION_SHELL_CHECKED = 1U << 3,
    UMI_APPLICATION_SHELL_MOVABLE = 1U << 4,
    UMI_APPLICATION_SHELL_CLOSABLE = 1U << 5,
    UMI_APPLICATION_SHELL_MULTI_INSTANCE = 1U << 6,
    UMI_APPLICATION_SHELL_CONTEXT_AWARE = 1U << 7,
    UMI_APPLICATION_SHELL_REQUIRES_TRUST = 1U << 8,
    UMI_APPLICATION_SHELL_PINNABLE = 1U << 9
};

typedef struct UmiApplicationShellContribution {
    uint32_t structure_size;
    uint32_t api_version;
    char contribution_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char title[UMI_APPLICATION_SHELL_TITLE_CAPACITY];
    char description[UMI_APPLICATION_SHELL_TEXT_CAPACITY];
    char command_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char component_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char group_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char icon_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char context_key[UMI_APPLICATION_SHELL_ID_CAPACITY];
    UmiApplicationShellRole role;
    UmiApplicationShellRegion region;
    int32_t order;
    UmiApplicationShellFlags flags;
    uint32_t badge_count;
    uint64_t revision;
} UmiApplicationShellContribution;

void umi_application_shell_contribution_init(
    UmiApplicationShellContribution *contribution,
    const char *contribution_id,
    const char *title,
    UmiApplicationShellRole role,
    UmiApplicationShellRegion region);

UmiStatus umi_application_shell_contribution_validate(
    const UmiApplicationShellContribution *contribution);

const char *umi_application_shell_role_text(UmiApplicationShellRole role);
const char *umi_application_shell_region_text(UmiApplicationShellRegion region);

#ifdef __cplusplus
}
#endif

#endif
