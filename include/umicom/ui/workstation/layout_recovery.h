/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_recovery.h
 *
 * PURPOSE:
 *   Define missing-monitor and invalid-geometry recovery for portable saved workstation layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_RECOVERY_H
#define UMICOM_UI_WORKSTATION_LAYOUT_RECOVERY_H

#include "umicom/ui/workstation/floating_window.h"
#include "umicom/ui/workstation/monitor_topology.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsLayoutRecoveryResult {
    char chosen_monitor_id[UMI_UI_ID_CAPACITY];
    UmiUiRect recovered_bounds;
    bool monitor_changed;
    bool geometry_changed;
} UmiWsLayoutRecoveryResult;

UmiStatus umi_ws_layout_recover_window(const UmiWsFloatingWindow *window,
                                       const UmiWsMonitorTopology *topology,
                                       UmiWsLayoutRecoveryResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
