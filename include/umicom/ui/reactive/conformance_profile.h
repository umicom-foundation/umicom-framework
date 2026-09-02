/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/conformance_profile.h
 *
 * PURPOSE:
 *   Define reactive-state conformance requirements shared by all frontend renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CONFORMANCE_PROFILE_H
#define UMICOM_UI_REACTIVE_CONFORMANCE_PROFILE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive conformance profile data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveConformanceProfile {
    bool two_way_binding;
    bool validation;
    bool computed_state;
    bool cycle_detection;
    bool transactional_updates;
    bool renderer_neutral;
} UmiUiReactiveConformanceProfile;
/**
 * Provide the ui reactive conformance profile strict operation used by this module and its
 * client applications.
 */
UmiUiReactiveConformanceProfile umi_ui_reactive_conformance_profile_strict(void);
/**
 * Provide the ui reactive conformance profile complete operation used by this module and
 * its client applications.
 */
int umi_ui_reactive_conformance_profile_complete(const UmiUiReactiveConformanceProfile *item);
#ifdef __cplusplus
}
#endif
#endif
