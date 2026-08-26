/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/types.c
 *
 * PURPOSE:
 *   Implement bounded adaptive-shell text helpers and stable shared enum utilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/adaptive/types.h"
#include <string.h>

/* Copy text while rejecting truncation so persisted identifiers remain stable. */
UmiStatus umi_adaptive_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Convert a device-family enum to a stable diagnostics/persistence label. */
const char *umi_adaptive_device_family_name(UmiAdaptiveDeviceFamily value)
{
    switch (value) {
    case UMI_ADAPTIVE_DEVICE_DESKTOP: return "desktop";
    case UMI_ADAPTIVE_DEVICE_LAPTOP: return "laptop";
    case UMI_ADAPTIVE_DEVICE_TABLET: return "tablet";
    case UMI_ADAPTIVE_DEVICE_HANDSET: return "handset";
    case UMI_ADAPTIVE_DEVICE_LARGE_DISPLAY: return "large-display";
    case UMI_ADAPTIVE_DEVICE_FOLDABLE: return "foldable";
    default: return "unknown";
    }
}

/* Convert a navigation-pattern enum to a stable renderer-neutral label. */
const char *umi_adaptive_navigation_pattern_name(UmiAdaptiveNavigationPattern value)
{
    switch (value) {
    case UMI_ADAPTIVE_NAV_TOP_BAR: return "top-bar";
    case UMI_ADAPTIVE_NAV_SIDE_RAIL: return "side-rail";
    case UMI_ADAPTIVE_NAV_DRAWER: return "drawer";
    case UMI_ADAPTIVE_NAV_BOTTOM_BAR: return "bottom-bar";
    case UMI_ADAPTIVE_NAV_COMMAND: return "command";
    default: return "unknown";
    }
}

/* Determine portrait/landscape/square semantics from logical viewport bounds. */
UmiAdaptiveDisplayOrientation umi_adaptive_orientation_from_viewport(UmiAdaptiveViewport viewport)
{
    if (viewport.width == viewport.height) {
        return UMI_ADAPTIVE_ORIENTATION_SQUARE;
    }
    return viewport.width > viewport.height
        ? UMI_ADAPTIVE_ORIENTATION_LANDSCAPE
        : UMI_ADAPTIVE_ORIENTATION_PORTRAIT;
}
