/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/resize_policy.h
 *
 * PURPOSE:
 *   Define minimum, maximum and preferred sizing policy for dense professional workstation panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_RESIZE_POLICY_H
#define UMICOM_UI_WORKSTATION_RESIZE_POLICY_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsResizePolicy {
    UmiUiSize minimum;
    UmiUiSize preferred;
    UmiUiSize maximum;
} UmiWsResizePolicy;

UmiStatus umi_ws_resize_policy_init(UmiWsResizePolicy *policy,
                                    UmiUiSize minimum,
                                    UmiUiSize preferred,
                                    UmiUiSize maximum);
UmiUiSize umi_ws_resize_policy_apply(const UmiWsResizePolicy *policy, UmiUiSize requested);

#ifdef __cplusplus
}
#endif

#endif
