/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/adaptive_service.c
 *
 * PURPOSE:
 *   Expose aggregate adaptive-shell readiness and active-device state to Framework applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/adaptive_service.h"
#include <string.h>

/* Compose canonical device, size, density, navigation and shell resolution into one readiness snapshot. */
UmiStatus umi_adaptive_service_snapshot(const UmiAdaptiveShellProfile *profile,
                                        UmiAdaptiveViewport viewport,
                                        UmiAdaptiveInputMode input_mode,
                                        UmiAdaptiveServiceSnapshot *out_snapshot)
{
    UmiAdaptiveResolvedShell shell;
    UmiStatus status;
    int touch = input_mode == UMI_ADAPTIVE_INPUT_TOUCH || input_mode == UMI_ADAPTIVE_INPUT_HYBRID;
    int keyboard = input_mode != UMI_ADAPTIVE_INPUT_TOUCH;
    if (profile == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof *out_snapshot);
    status = umi_adaptive_viewport_class_resolve(viewport, &out_snapshot->size_class);
    if (status != UMI_STATUS_OK) return status;
    status = umi_adaptive_device_family_classify(viewport.width, touch, keyboard, &out_snapshot->family);
    if (status != UMI_STATUS_OK) return status;
    status = umi_adaptive_density_policy_resolve(out_snapshot->size_class, input_mode, &out_snapshot->density);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->navigation = umi_adaptive_navigation_pattern_select(out_snapshot->size_class, input_mode);
    status = umi_adaptive_shell_resolver_resolve(profile, out_snapshot->size_class, &shell);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->visible_regions = shell.inline_regions + shell.overlay_regions;
    out_snapshot->ready = 1;
    return UMI_STATUS_OK;
}
