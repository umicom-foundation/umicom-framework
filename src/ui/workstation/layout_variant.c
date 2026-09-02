/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_variant.c
 *
 * PURPOSE:
 *   Implement automatic layout-variant selection for laptops, ultrawide displays and multi-monitor workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/layout_variant.h"

/*
 * Provide the ws layout variant select operation used by this module and its client
 * applications.
 */
UmiWsLayoutVariantKind umi_ws_layout_variant_select(const UmiWsLayoutEnvironment *environment) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL) return UMI_WS_LAYOUT_DESKTOP;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (environment->monitor_count >= 3U) return UMI_WS_LAYOUT_TRIPLE_MONITOR;
    /* Apply this branch only when its contract condition is satisfied. */
    if (environment->monitor_count == 2U) return UMI_WS_LAYOUT_DUAL_MONITOR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (environment->primary_width >= 3000) return UMI_WS_LAYOUT_ULTRAWIDE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (environment->primary_width <= 1600 || environment->primary_height <= 900) return UMI_WS_LAYOUT_LAPTOP;
    return UMI_WS_LAYOUT_DESKTOP;
}

/*
 * Provide the ws layout variant text operation used by this module and its client
 * applications.
 */
const char *umi_ws_layout_variant_text(UmiWsLayoutVariantKind variant) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (variant) {
        case UMI_WS_LAYOUT_LAPTOP: return "laptop";
        case UMI_WS_LAYOUT_ULTRAWIDE: return "ultrawide";
        case UMI_WS_LAYOUT_DUAL_MONITOR: return "dual-monitor";
        case UMI_WS_LAYOUT_TRIPLE_MONITOR: return "triple-monitor";
        case UMI_WS_LAYOUT_DESKTOP:
        default: return "desktop";
    }
}
