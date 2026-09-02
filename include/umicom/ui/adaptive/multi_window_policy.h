/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/multi_window_policy.h
 *
 * PURPOSE:
 *   Govern detachable and multi-window application surfaces by device family and available width.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_MULTI_WINDOW_POLICY_H
#define UMICOM_UI_ADAPTIVE_MULTI_WINDOW_POLICY_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive multi window policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAdaptiveMultiWindowPolicy {
    int allow_detach;
    int allow_multiple_top_level;
    size_t maximum_windows;
} UmiAdaptiveMultiWindowPolicy;
/* Resolve multi-window capability for an adaptive device context. */
UmiAdaptiveMultiWindowPolicy umi_adaptive_multi_window_policy_resolve(UmiAdaptiveDeviceFamily family,
                                                                      UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
