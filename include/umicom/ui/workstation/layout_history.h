/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_history.h
 *
 * PURPOSE:
 *   Define bounded layout history navigation for compare, undo-like recovery and visual layout browser timelines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_HISTORY_H
#define UMICOM_UI_WORKSTATION_LAYOUT_HISTORY_H

#include <stddef.h>
#include "umicom/ui/workstation/layout_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WS_LAYOUT_HISTORY_MAX 32U

/**
 * Represent the ws layout history data shared with callers of this public contract.
 */
typedef struct UmiWsLayoutHistory {
    UmiWsLayoutSnapshot entries[UMI_WS_LAYOUT_HISTORY_MAX];
    size_t count;
    size_t cursor;
} UmiWsLayoutHistory;

/**
 * Initialise ws layout history from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_layout_history_init(UmiWsLayoutHistory *history);
/**
 * Provide the ws layout history push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_history_push(UmiWsLayoutHistory *history, const UmiWsLayoutSnapshot *snapshot);
/**
 * Provide the ws layout history back operation used by this module and its client
 * applications.
 */
const UmiWsLayoutSnapshot *umi_ws_layout_history_back(UmiWsLayoutHistory *history);
/**
 * Provide the ws layout history forward operation used by this module and its client
 * applications.
 */
const UmiWsLayoutSnapshot *umi_ws_layout_history_forward(UmiWsLayoutHistory *history);
/**
 * Provide the ws layout history current operation used by this module and its client
 * applications.
 */
const UmiWsLayoutSnapshot *umi_ws_layout_history_current(const UmiWsLayoutHistory *history);

#ifdef __cplusplus
}
#endif

#endif
