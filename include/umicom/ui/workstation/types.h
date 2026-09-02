/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/types.h
 *
 * PURPOSE:
 *   Define stable toolkit-neutral types shared by reusable workstation layouts, surfaces and GTK4 adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_WORKSTATION_TYPES_H
#define UMICOM_UI_WORKSTATION_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WS_API_VERSION 1U
#define UMI_WS_MAX_SURFACES 64U
#define UMI_WS_MAX_LAYOUT_ITEMS 64U
#define UMI_WS_MAX_LAYOUTS 32U
#define UMI_WS_MAX_PERSPECTIVES 32U
#define UMI_WS_MAX_TABS 32U
#define UMI_WS_MAX_MONITORS 16U
#define UMI_WS_MAX_TOOLBAR_ITEMS 48U
#define UMI_WS_MAX_STATUS_ITEMS 32U
#define UMI_WS_MAX_PROPERTIES 64U
#define UMI_WS_MAX_PALETTE_ITEMS 96U
#define UMI_WS_MAX_ASSETS 128U
#define UMI_WS_MAX_TRACKS 32U
#define UMI_WS_MAX_CLIPS 128U
#define UMI_WS_MAX_GRAPH_NODES 96U
#define UMI_WS_MAX_GRAPH_EDGES 192U
#define UMI_WS_MAX_GRID_COLUMNS 32U
#define UMI_WS_MAX_WORKSPACES 24U

/**
 * List the named ws application domain values accepted by this public contract.
 */
typedef enum UmiWsApplicationDomain {
    UMI_WS_DOMAIN_GENERIC = 0,
    UMI_WS_DOMAIN_STUDIO = 1,
    UMI_WS_DOMAIN_TRADER = 2,
    UMI_WS_DOMAIN_VIDEO_EDITOR = 3,
    UMI_WS_DOMAIN_AI_CREATOR = 4,
    UMI_WS_DOMAIN_MUSIC_STUDIO = 5,
    UMI_WS_DOMAIN_VIDEO_STUDIO = 6,
    UMI_WS_DOMAIN_OS = 7,
    UMI_WS_DOMAIN_KITCHEN_DESIGNER = 8,
    UMI_WS_DOMAIN_OPERATIONS = 9
} UmiWsApplicationDomain;

/**
 * List the named ws surface kind values accepted by this public contract.
 */
typedef enum UmiWsSurfaceKind {
    UMI_WS_SURFACE_PANEL = 1,
    UMI_WS_SURFACE_EDITOR = 2,
    UMI_WS_SURFACE_CANVAS = 3,
    UMI_WS_SURFACE_CHART = 4,
    UMI_WS_SURFACE_GRID = 5,
    UMI_WS_SURFACE_TIMELINE = 6,
    UMI_WS_SURFACE_MEDIA = 7,
    UMI_WS_SURFACE_INSPECTOR = 8,
    UMI_WS_SURFACE_PALETTE = 9,
    UMI_WS_SURFACE_TERMINAL = 10,
    UMI_WS_SURFACE_GRAPH = 11,
    UMI_WS_SURFACE_WAVEFORM = 12,
    UMI_WS_SURFACE_BROWSER = 13,
    UMI_WS_SURFACE_STATUS = 14,
    UMI_WS_SURFACE_TOOLBAR = 15
} UmiWsSurfaceKind;

/**
 * List the named ws dock region values accepted by this public contract.
 */
typedef enum UmiWsDockRegion {
    UMI_WS_DOCK_LEFT = 1,
    UMI_WS_DOCK_RIGHT = 2,
    UMI_WS_DOCK_TOP = 3,
    UMI_WS_DOCK_BOTTOM = 4,
    UMI_WS_DOCK_CENTRE = 5,
    UMI_WS_DOCK_FLOATING = 6
} UmiWsDockRegion;

/**
 * List the named ws visibility values accepted by this public contract.
 */
typedef enum UmiWsVisibility {
    UMI_WS_VISIBILITY_HIDDEN = 0,
    UMI_WS_VISIBILITY_VISIBLE = 1,
    UMI_WS_VISIBILITY_AUTO_HIDE = 2
} UmiWsVisibility;

/**
 * List the named ws density values accepted by this public contract.
 */
typedef enum UmiWsDensity {
    UMI_WS_DENSITY_COMFORTABLE = 1,
    UMI_WS_DENSITY_COMPACT = 2,
    UMI_WS_DENSITY_TOUCH = 3
} UmiWsDensity;

/**
 * List the named ws layout variant kind values accepted by this public contract.
 */
typedef enum UmiWsLayoutVariantKind {
    UMI_WS_LAYOUT_LAPTOP = 1,
    UMI_WS_LAYOUT_DESKTOP = 2,
    UMI_WS_LAYOUT_ULTRAWIDE = 3,
    UMI_WS_LAYOUT_DUAL_MONITOR = 4,
    UMI_WS_LAYOUT_TRIPLE_MONITOR = 5
} UmiWsLayoutVariantKind;

/* Copy bounded text and reject silent truncation. */
UmiStatus umi_ws_copy_text(char *destination, size_t capacity, const char *source);
/* Validate a stable UI identifier against Framework identifier capacity. */
bool umi_ws_id_valid(const char *identifier);
/* Clamp a scalar into the inclusive 0..1 range. */
double umi_ws_clamp_unit(double value);
/* Clamp an integer into a caller-provided inclusive range. */
int32_t umi_ws_clamp_i32(int32_t value, int32_t minimum, int32_t maximum);
/* Produce a deterministic lightweight fingerprint for layout evidence. */
uint64_t umi_ws_hash_text(const char *text);
/**
 * Provide the ws domain text operation used by this module and its client applications.
 */
const char *umi_ws_domain_text(UmiWsApplicationDomain domain);
/**
 * Provide the ws surface kind text operation used by this module and its client
 * applications.
 */
const char *umi_ws_surface_kind_text(UmiWsSurfaceKind kind);
/**
 * Provide the ws dock region text operation used by this module and its client
 * applications.
 */
const char *umi_ws_dock_region_text(UmiWsDockRegion region);

#ifdef __cplusplus
}
#endif
#endif
