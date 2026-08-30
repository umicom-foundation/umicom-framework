/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/navigation_pattern.h
 *
 * PURPOSE:
 *   Select an adaptive navigation pattern from canonical size class and input mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_NAVIGATION_PATTERN_H
#define UMICOM_UI_ADAPTIVE_NAVIGATION_PATTERN_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Select navigation appropriate to the available size and primary input mode. */
UmiAdaptiveNavigationPattern umi_adaptive_navigation_pattern_select(UmiDesignSizeClass size_class,
                                                                    UmiAdaptiveInputMode input_mode);

#ifdef __cplusplus
}
#endif
#endif
