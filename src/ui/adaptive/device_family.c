/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/device_family.c
 *
 * PURPOSE:
 *   Classify logical viewport/input combinations into reusable device-family semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/device_family.h"

/* Classify a device family using stable logical-width thresholds and capabilities. */
UmiStatus umi_adaptive_device_family_classify(int32_t logical_width,
                                               int touch,
                                               int keyboard,
                                               UmiAdaptiveDeviceFamily *out_family)
{
    if (logical_width <= 0 || out_family == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (logical_width < 600) {
        *out_family = UMI_ADAPTIVE_DEVICE_HANDSET;
    } else if (logical_width < 1024 && touch != 0) {
        *out_family = UMI_ADAPTIVE_DEVICE_TABLET;
    } else if (logical_width < 1600 && keyboard != 0) {
        *out_family = UMI_ADAPTIVE_DEVICE_LAPTOP;
    } else if (logical_width >= 2560) {
        *out_family = UMI_ADAPTIVE_DEVICE_LARGE_DISPLAY;
    } else {
        *out_family = UMI_ADAPTIVE_DEVICE_DESKTOP;
    }
    return UMI_STATUS_OK;
}
