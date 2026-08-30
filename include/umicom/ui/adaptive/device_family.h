/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/device_family.h
 *
 * PURPOSE:
 *   Classify logical viewport/input combinations into reusable device-family semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_DEVICE_FAMILY_H
#define UMICOM_UI_ADAPTIVE_DEVICE_FAMILY_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Classify the active device family from logical width and input capabilities. */
UmiStatus umi_adaptive_device_family_classify(int32_t logical_width,
                                               int touch,
                                               int keyboard,
                                               UmiAdaptiveDeviceFamily *out_family);

#ifdef __cplusplus
}
#endif
#endif
