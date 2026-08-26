/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/viewport_class.h
 *
 * PURPOSE:
 *   Bridge adaptive logical viewports to the canonical Design-System size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_VIEWPORT_CLASS_H
#define UMICOM_UI_ADAPTIVE_VIEWPORT_CLASS_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Resolve canonical Framework size class from the current logical viewport. */
UmiStatus umi_adaptive_viewport_class_resolve(UmiAdaptiveViewport viewport,
                                              UmiDesignSizeClass *out_class);

#ifdef __cplusplus
}
#endif
#endif
