/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/types.c
 *
 * PURPOSE:
 *   Implement bounded workstation utility logic shared by semantic UI models and frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/workstation/types.h"

/* Copy character-by-character so truncation is explicit and deterministic. */
UmiStatus umi_ws_copy_text(char *destination, size_t capacity, const char *source) {
    size_t index = 0U;
    if (destination == NULL || source == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    while (source[index] != '\0' && index + 1U < capacity) {
        destination[index] = source[index];
        ++index;
    }
    if (source[index] != '\0') {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    destination[index] = '\0';
    return UMI_STATUS_OK;
}

/* Accept only non-empty identifiers terminated inside the shared UI capacity. */
bool umi_ws_id_valid(const char *identifier) {
    size_t index = 0U;
    if (identifier == NULL || identifier[0] == '\0') {
        return false;
    }
    while (index < UMI_UI_ID_CAPACITY) {
        if (identifier[index] == '\0') {
            return true;
        }
        ++index;
    }
    return false;
}

/* Normalise scores and ratios used by toolkit-neutral layout policies. */
double umi_ws_clamp_unit(double value) {
    if (value < 0.0) return 0.0;
    if (value > 1.0) return 1.0;
    return value;
}

/* Keep geometry and spacing values inside explicit policy limits. */
int32_t umi_ws_clamp_i32(int32_t value, int32_t minimum, int32_t maximum) {
    if (minimum > maximum) return minimum;
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

/* Use FNV-1a for deterministic layout/preset fingerprints without hidden state. */
uint64_t umi_ws_hash_text(const char *text) {
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t index = 0U;
    if (text == NULL) return 0U;
    while (text[index] != '\0') {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
        ++index;
    }
    return hash;
}

const char *umi_ws_domain_text(UmiWsApplicationDomain domain) {
    switch (domain) {
        case UMI_WS_DOMAIN_STUDIO: return "studio";
        case UMI_WS_DOMAIN_TRADER: return "trader";
        case UMI_WS_DOMAIN_VIDEO_EDITOR: return "video-editor";
        case UMI_WS_DOMAIN_AI_CREATOR: return "ai-creator";
        case UMI_WS_DOMAIN_MUSIC_STUDIO: return "music-studio";
        case UMI_WS_DOMAIN_VIDEO_STUDIO: return "video-studio";
        case UMI_WS_DOMAIN_OS: return "os";
        case UMI_WS_DOMAIN_KITCHEN_DESIGNER: return "kitchen-designer";
        case UMI_WS_DOMAIN_OPERATIONS: return "operations";
        case UMI_WS_DOMAIN_GENERIC:
        default: return "generic";
    }
}

const char *umi_ws_surface_kind_text(UmiWsSurfaceKind kind) {
    switch (kind) {
        case UMI_WS_SURFACE_EDITOR: return "editor";
        case UMI_WS_SURFACE_CANVAS: return "canvas";
        case UMI_WS_SURFACE_CHART: return "chart";
        case UMI_WS_SURFACE_GRID: return "grid";
        case UMI_WS_SURFACE_TIMELINE: return "timeline";
        case UMI_WS_SURFACE_MEDIA: return "media";
        case UMI_WS_SURFACE_INSPECTOR: return "inspector";
        case UMI_WS_SURFACE_PALETTE: return "palette";
        case UMI_WS_SURFACE_TERMINAL: return "terminal";
        case UMI_WS_SURFACE_GRAPH: return "graph";
        case UMI_WS_SURFACE_WAVEFORM: return "waveform";
        case UMI_WS_SURFACE_BROWSER: return "browser";
        case UMI_WS_SURFACE_STATUS: return "status";
        case UMI_WS_SURFACE_TOOLBAR: return "toolbar";
        case UMI_WS_SURFACE_PANEL:
        default: return "panel";
    }
}

const char *umi_ws_dock_region_text(UmiWsDockRegion region) {
    switch (region) {
        case UMI_WS_DOCK_LEFT: return "left";
        case UMI_WS_DOCK_RIGHT: return "right";
        case UMI_WS_DOCK_TOP: return "top";
        case UMI_WS_DOCK_BOTTOM: return "bottom";
        case UMI_WS_DOCK_FLOATING: return "floating";
        case UMI_WS_DOCK_CENTRE:
        default: return "centre";
    }
}
