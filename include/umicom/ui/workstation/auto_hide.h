/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/auto_hide.h
 *
 * PURPOSE:
 *   Define auto-hide edge-rail state without destroying the stored layout graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_AUTO_HIDE_H
#define UMICOM_UI_WORKSTATION_AUTO_HIDE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws auto hide state data shared with callers of this public contract.
 */
typedef struct UmiWsAutoHideState {
    char surface_id[UMI_UI_ID_CAPACITY];
    UmiWsDockRegion edge;
    bool enabled;
    bool revealed;
    int32_t reveal_extent;
} UmiWsAutoHideState;

/**
 * Initialise ws auto hide from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ws_auto_hide_init(UmiWsAutoHideState *state, const char *surface_id, UmiWsDockRegion edge);
/**
 * Provide the ws auto hide set revealed operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_auto_hide_set_revealed(UmiWsAutoHideState *state, bool revealed);
/**
 * Check that ws auto hide edge satisfies its contract before another service relies on it.
 */
bool umi_ws_auto_hide_edge_valid(UmiWsDockRegion edge);

#ifdef __cplusplus
}
#endif

#endif
