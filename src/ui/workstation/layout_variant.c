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

UmiWsLayoutVariantKind umi_ws_layout_variant_select(const UmiWsLayoutEnvironment *environment) {
    if (environment == NULL) return UMI_WS_LAYOUT_DESKTOP;
    if (environment->monitor_count >= 3U) return UMI_WS_LAYOUT_TRIPLE_MONITOR;
    if (environment->monitor_count == 2U) return UMI_WS_LAYOUT_DUAL_MONITOR;
    if (environment->primary_width >= 3000) return UMI_WS_LAYOUT_ULTRAWIDE;
    if (environment->primary_width <= 1600 || environment->primary_height <= 900) return UMI_WS_LAYOUT_LAPTOP;
    return UMI_WS_LAYOUT_DESKTOP;
}

const char *umi_ws_layout_variant_text(UmiWsLayoutVariantKind variant) {
    switch (variant) {
        case UMI_WS_LAYOUT_LAPTOP: return "laptop";
        case UMI_WS_LAYOUT_ULTRAWIDE: return "ultrawide";
        case UMI_WS_LAYOUT_DUAL_MONITOR: return "dual-monitor";
        case UMI_WS_LAYOUT_TRIPLE_MONITOR: return "triple-monitor";
        case UMI_WS_LAYOUT_DESKTOP:
        default: return "desktop";
    }
}
