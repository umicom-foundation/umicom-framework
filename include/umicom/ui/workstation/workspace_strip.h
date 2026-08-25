/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/workspace_strip.h
 *
 * PURPOSE:
 *   Define bottom/top workspace strip state for rapid switching between named layouts and perspectives.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_WORKSPACE_STRIP_H
#define UMICOM_UI_WORKSTATION_WORKSPACE_STRIP_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsWorkspaceEntry {
    char workspace_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char layout_id[UMI_UI_ID_CAPACITY];
    bool pinned;
} UmiWsWorkspaceEntry;

typedef struct UmiWsWorkspaceStrip {
    UmiWsWorkspaceEntry entries[UMI_WS_MAX_WORKSPACES];
    size_t count;
    size_t active_index;
} UmiWsWorkspaceStrip;

void umi_ws_workspace_strip_init(UmiWsWorkspaceStrip *strip);
UmiStatus umi_ws_workspace_strip_add(UmiWsWorkspaceStrip *strip,
                                     const char *workspace_id,
                                     const char *label,
                                     const char *layout_id);
UmiStatus umi_ws_workspace_strip_activate(UmiWsWorkspaceStrip *strip, const char *workspace_id);
const UmiWsWorkspaceEntry *umi_ws_workspace_strip_active(const UmiWsWorkspaceStrip *strip);

#ifdef __cplusplus
}
#endif

#endif
