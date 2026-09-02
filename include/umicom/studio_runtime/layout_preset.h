/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/layout_preset.h
 *
 * PURPOSE:
 *   Define semantic Studio layout presets over Framework surface bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_LAYOUT_PRESET_H
#define UMICOM_STUDIO_RUNTIME_LAYOUT_PRESET_H

#include "umicom/studio_runtime/bindings.h"
#include "umicom/studio_runtime/surface_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_LAYOUT_PRESET_SURFACE_CAPACITY 16U

/**
 * Represent the studio runtime layout preset definition data shared with callers of this
 * public contract.
 */
typedef struct UmiStudioRuntimeLayoutPresetDefinition {
    const char *preset_id;
    const char *title;
    const char *description;
    const char *perspective_id;
    const UmiStudioRuntimeSurfaceKind *visible_surfaces;
    size_t visible_surface_count;
    int primary_sidebar_visible;
    int secondary_sidebar_visible;
    int bottom_panel_visible;
    int status_bar_visible;
    int zen_mode;
} UmiStudioRuntimeLayoutPresetDefinition;

/**
 * Check that studio layout preset satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_studio_layout_preset_validate(
    const UmiStudioRuntimeLayoutPresetDefinition *preset);
/* Report whether a preset explicitly shows one semantic Studio surface. */
int umi_studio_layout_preset_contains_surface(
    const UmiStudioRuntimeLayoutPresetDefinition *preset,
    UmiStudioRuntimeSurfaceKind kind);

/**
 * Perform studio layout preset through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_studio_layout_preset_apply(
    UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeLayoutPresetDefinition *preset);

#ifdef __cplusplus
}
#endif
#endif
