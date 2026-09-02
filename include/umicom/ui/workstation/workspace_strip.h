/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/workspace_strip.h
 *
 * PURPOSE:
 *   Define bottom/top workspace strip state for rapid switching between named layouts and perspectives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_WORKSPACE_STRIP_H
#define UMICOM_UI_WORKSTATION_WORKSPACE_STRIP_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws workspace entry data shared with callers of this public contract.
 */
typedef struct UmiWsWorkspaceEntry {
    char workspace_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char layout_id[UMI_UI_ID_CAPACITY];
    bool pinned;
} UmiWsWorkspaceEntry;

/**
 * Represent the ws workspace strip data shared with callers of this public contract.
 */
typedef struct UmiWsWorkspaceStrip {
    UmiWsWorkspaceEntry entries[UMI_WS_MAX_WORKSPACES];
    size_t count;
    size_t active_index;
} UmiWsWorkspaceStrip;

/**
 * Initialise ws workspace strip from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_workspace_strip_init(UmiWsWorkspaceStrip *strip);
/**
 * Add ws workspace strip only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ws_workspace_strip_add(UmiWsWorkspaceStrip *strip,
                                     const char *workspace_id,
                                     const char *label,
                                     const char *layout_id);
/**
 * Provide the ws workspace strip activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_workspace_strip_activate(UmiWsWorkspaceStrip *strip, const char *workspace_id);
/**
 * Provide the ws workspace strip active operation used by this module and its client
 * applications.
 */
const UmiWsWorkspaceEntry *umi_ws_workspace_strip_active(const UmiWsWorkspaceStrip *strip);

#ifdef __cplusplus
}
#endif

#endif
