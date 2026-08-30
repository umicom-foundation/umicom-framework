/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/layout.h
 *
 * PURPOSE:
 *   Store reusable shell placements independently from physical pixels, toolkit
 *   widgets and monitor APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_LAYOUT_H
#define UMICOM_APPLICATION_SHELL_LAYOUT_H

#include "umicom/application_shell/drag_drop.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_SHELL_LAYOUT_MAX_ITEMS 256U

typedef struct UmiApplicationShellPlacement {
    char contribution_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char container_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    UmiApplicationShellRegion region;
    size_t order;
    int visible;
} UmiApplicationShellPlacement;

typedef struct UmiApplicationShellLayout {
    char layout_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char title[UMI_APPLICATION_SHELL_TITLE_CAPACITY];
    UmiApplicationShellPlacement
        placements[UMI_APPLICATION_SHELL_LAYOUT_MAX_ITEMS];
    size_t placement_count;
    uint64_t revision;
} UmiApplicationShellLayout;

void umi_application_shell_layout_init(
    UmiApplicationShellLayout *layout,
    const char *layout_id,
    const char *title);

UmiStatus umi_application_shell_layout_place(
    UmiApplicationShellLayout *layout,
    const char *contribution_id,
    const char *container_id,
    UmiApplicationShellRegion region,
    size_t order,
    int visible);

UmiStatus umi_application_shell_layout_move(
    UmiApplicationShellLayout *layout,
    const UmiApplicationShellDropTransaction *transaction);

UmiStatus umi_application_shell_layout_find(
    const UmiApplicationShellLayout *layout,
    const char *contribution_id,
    UmiApplicationShellPlacement *out_placement);

#ifdef __cplusplus
}
#endif

#endif
