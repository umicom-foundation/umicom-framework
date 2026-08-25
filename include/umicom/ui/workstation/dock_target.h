/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/dock_target.h
 *
 * PURPOSE:
 *   Define scored docking targets so GTK4 drag overlays and keyboard docking share one deterministic policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DOCK_TARGET_H
#define UMICOM_UI_WORKSTATION_DOCK_TARGET_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsDockTarget {
    char target_id[UMI_UI_ID_CAPACITY];
    UmiWsDockRegion region;
    UmiUiRect bounds;
    bool enabled;
    int32_t priority;
} UmiWsDockTarget;

UmiStatus umi_ws_dock_target_init(UmiWsDockTarget *target,
                                  const char *target_id,
                                  UmiWsDockRegion region,
                                  UmiUiRect bounds);
int32_t umi_ws_dock_target_score(const UmiWsDockTarget *target, UmiUiPoint pointer);
bool umi_ws_dock_target_contains(const UmiWsDockTarget *target, UmiUiPoint pointer);

#ifdef __cplusplus
}
#endif

#endif
