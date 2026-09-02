/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/resize_policy.h
 *
 * PURPOSE:
 *   Define minimum, maximum and preferred sizing policy for dense professional workstation panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_RESIZE_POLICY_H
#define UMICOM_UI_WORKSTATION_RESIZE_POLICY_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws resize policy data shared with callers of this public contract.
 */
typedef struct UmiWsResizePolicy {
    UmiUiSize minimum;
    UmiUiSize preferred;
    UmiUiSize maximum;
} UmiWsResizePolicy;

/**
 * Initialise ws resize policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_resize_policy_init(UmiWsResizePolicy *policy,
                                    UmiUiSize minimum,
                                    UmiUiSize preferred,
                                    UmiUiSize maximum);
/**
 * Perform ws resize policy through the module contract so client applications do not
 * duplicate its policy.
 */
UmiUiSize umi_ws_resize_policy_apply(const UmiWsResizePolicy *policy, UmiUiSize requested);

#ifdef __cplusplus
}
#endif

#endif
