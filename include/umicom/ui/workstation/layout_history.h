/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_history.h
 *
 * PURPOSE:
 *   Define bounded layout history navigation for compare, undo-like recovery and visual layout browser timelines.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_HISTORY_H
#define UMICOM_UI_WORKSTATION_LAYOUT_HISTORY_H

#include <stddef.h>
#include "umicom/ui/workstation/layout_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WS_LAYOUT_HISTORY_MAX 32U

typedef struct UmiWsLayoutHistory {
    UmiWsLayoutSnapshot entries[UMI_WS_LAYOUT_HISTORY_MAX];
    size_t count;
    size_t cursor;
} UmiWsLayoutHistory;

void umi_ws_layout_history_init(UmiWsLayoutHistory *history);
UmiStatus umi_ws_layout_history_push(UmiWsLayoutHistory *history, const UmiWsLayoutSnapshot *snapshot);
const UmiWsLayoutSnapshot *umi_ws_layout_history_back(UmiWsLayoutHistory *history);
const UmiWsLayoutSnapshot *umi_ws_layout_history_forward(UmiWsLayoutHistory *history);
const UmiWsLayoutSnapshot *umi_ws_layout_history_current(const UmiWsLayoutHistory *history);

#ifdef __cplusplus
}
#endif

#endif
