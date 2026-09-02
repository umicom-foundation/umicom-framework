/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/dock_target.h
 *
 * PURPOSE:
 *   Define scored docking targets so GTK4 drag overlays and keyboard docking share one deterministic policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DOCK_TARGET_H
#define UMICOM_UI_WORKSTATION_DOCK_TARGET_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws dock target data shared with callers of this public contract.
 */
typedef struct UmiWsDockTarget {
    char target_id[UMI_UI_ID_CAPACITY];
    UmiWsDockRegion region;
    UmiUiRect bounds;
    bool enabled;
    int32_t priority;
} UmiWsDockTarget;

/**
 * Initialise ws dock target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_dock_target_init(UmiWsDockTarget *target,
                                  const char *target_id,
                                  UmiWsDockRegion region,
                                  UmiUiRect bounds);
/**
 * Provide the ws dock target score operation used by this module and its client
 * applications.
 */
int32_t umi_ws_dock_target_score(const UmiWsDockTarget *target, UmiUiPoint pointer);
/**
 * Provide the ws dock target contains operation used by this module and its client
 * applications.
 */
bool umi_ws_dock_target_contains(const UmiWsDockTarget *target, UmiUiPoint pointer);

#ifdef __cplusplus
}
#endif

#endif
