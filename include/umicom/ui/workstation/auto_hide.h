/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/auto_hide.h
 *
 * PURPOSE:
 *   Define auto-hide edge-rail state without destroying the stored layout graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_AUTO_HIDE_H
#define UMICOM_UI_WORKSTATION_AUTO_HIDE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsAutoHideState {
    char surface_id[UMI_UI_ID_CAPACITY];
    UmiWsDockRegion edge;
    bool enabled;
    bool revealed;
    int32_t reveal_extent;
} UmiWsAutoHideState;

UmiStatus umi_ws_auto_hide_init(UmiWsAutoHideState *state, const char *surface_id, UmiWsDockRegion edge);
UmiStatus umi_ws_auto_hide_set_revealed(UmiWsAutoHideState *state, bool revealed);
bool umi_ws_auto_hide_edge_valid(UmiWsDockRegion edge);

#ifdef __cplusplus
}
#endif

#endif
