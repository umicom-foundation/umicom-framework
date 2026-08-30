/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/safe_area_resolver.h
 *
 * PURPOSE:
 *   Apply runtime safe-area semantics to authored designer component rectangles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_SAFE_AREA_RESOLVER_H
#define UMICOM_DESIGNER_ADAPTIVE_SAFE_AREA_RESOLVER_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Resolve an authored component rectangle into the current safe content area. */
UmiStatus umi_designer_safe_area_resolver_apply(UmiRadRect authored_rect,
                                                UmiAdaptiveViewport viewport,
                                                UmiAdaptiveInsets safe_area,
                                                UmiRadRect *out_rect);

#ifdef __cplusplus
}
#endif
#endif
