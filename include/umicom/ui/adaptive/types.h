/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/types.h
 *
 * PURPOSE:
 *   Define stable toolkit-neutral values shared by the adaptive application shell,
 *   responsive runtime policies and multi-device designer services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_TYPES_H
#define UMICOM_UI_ADAPTIVE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/ui/types.h"
#include "umicom/ui/design/density.h"
#include "umicom/ui/design/size_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_ADAPTIVE_ID_CAPACITY 128U
#define UMI_ADAPTIVE_TEXT_CAPACITY 256U
#define UMI_ADAPTIVE_MAX_ITEMS 64U
#define UMI_ADAPTIVE_MAX_BREAKPOINTS 16U

typedef enum UmiAdaptiveDeviceFamily {
    UMI_ADAPTIVE_DEVICE_DESKTOP = 1,
    UMI_ADAPTIVE_DEVICE_LAPTOP = 2,
    UMI_ADAPTIVE_DEVICE_TABLET = 3,
    UMI_ADAPTIVE_DEVICE_HANDSET = 4,
    UMI_ADAPTIVE_DEVICE_LARGE_DISPLAY = 5,
    UMI_ADAPTIVE_DEVICE_FOLDABLE = 6
} UmiAdaptiveDeviceFamily;

typedef enum UmiAdaptiveFormFactor {
    UMI_ADAPTIVE_FORM_TALL = 1,
    UMI_ADAPTIVE_FORM_STANDARD = 2,
    UMI_ADAPTIVE_FORM_WIDE = 3,
    UMI_ADAPTIVE_FORM_ULTRAWIDE = 4
} UmiAdaptiveFormFactor;

typedef enum UmiAdaptiveInputMode {
    UMI_ADAPTIVE_INPUT_POINTER = 1,
    UMI_ADAPTIVE_INPUT_TOUCH = 2,
    UMI_ADAPTIVE_INPUT_HYBRID = 3,
    UMI_ADAPTIVE_INPUT_KEYBOARD = 4
} UmiAdaptiveInputMode;

typedef enum UmiAdaptiveDisplayOrientation {
    UMI_ADAPTIVE_ORIENTATION_PORTRAIT = 1,
    UMI_ADAPTIVE_ORIENTATION_LANDSCAPE = 2,
    UMI_ADAPTIVE_ORIENTATION_SQUARE = 3
} UmiAdaptiveDisplayOrientation;

typedef enum UmiAdaptiveNavigationPattern {
    UMI_ADAPTIVE_NAV_TOP_BAR = 1,
    UMI_ADAPTIVE_NAV_SIDE_RAIL = 2,
    UMI_ADAPTIVE_NAV_DRAWER = 3,
    UMI_ADAPTIVE_NAV_BOTTOM_BAR = 4,
    UMI_ADAPTIVE_NAV_COMMAND = 5
} UmiAdaptiveNavigationPattern;

typedef enum UmiAdaptiveRegionRole {
    UMI_ADAPTIVE_REGION_HEADER = 1,
    UMI_ADAPTIVE_REGION_NAVIGATION = 2,
    UMI_ADAPTIVE_REGION_PRIMARY = 3,
    UMI_ADAPTIVE_REGION_SECONDARY = 4,
    UMI_ADAPTIVE_REGION_INSPECTOR = 5,
    UMI_ADAPTIVE_REGION_STATUS = 6,
    UMI_ADAPTIVE_REGION_OVERLAY = 7
} UmiAdaptiveRegionRole;

typedef enum UmiAdaptivePresentation {
    UMI_ADAPTIVE_PRESENTATION_HIDDEN = 0,
    UMI_ADAPTIVE_PRESENTATION_INLINE = 1,
    UMI_ADAPTIVE_PRESENTATION_COLLAPSED = 2,
    UMI_ADAPTIVE_PRESENTATION_OVERLAY = 3
} UmiAdaptivePresentation;

typedef struct UmiAdaptiveInsets {
    int32_t top;
    int32_t right;
    int32_t bottom;
    int32_t left;
} UmiAdaptiveInsets;

typedef struct UmiAdaptiveViewport {
    int32_t width;
    int32_t height;
} UmiAdaptiveViewport;

/* Copy text into a bounded adaptive-shell ABI buffer without silent truncation. */
UmiStatus umi_adaptive_copy_text(char *destination, size_t capacity, const char *source);
/* Return a stable public name for a device-family value. */
const char *umi_adaptive_device_family_name(UmiAdaptiveDeviceFamily value);
/* Return a stable public name for an adaptive navigation-pattern value. */
const char *umi_adaptive_navigation_pattern_name(UmiAdaptiveNavigationPattern value);
/* Classify a viewport orientation without toolkit-specific window APIs. */
UmiAdaptiveDisplayOrientation umi_adaptive_orientation_from_viewport(UmiAdaptiveViewport viewport);

#ifdef __cplusplus
}
#endif
#endif
