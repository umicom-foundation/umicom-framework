/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/safe_area.h
 *
 * PURPOSE:
 *   Resolve platform safe-area insets into renderer-neutral logical content bounds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_SAFE_AREA_H
#define UMICOM_UI_ADAPTIVE_SAFE_AREA_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Validate non-negative logical safe-area insets. */
int umi_adaptive_safe_area_valid(UmiAdaptiveInsets insets);
/* Apply safe-area insets to a viewport and return usable logical content bounds. */
UmiStatus umi_adaptive_safe_area_content_rect(UmiAdaptiveViewport viewport,
                                              UmiAdaptiveInsets insets,
                                              UmiUiRect *out_rect);

#ifdef __cplusplus
}
#endif
#endif
