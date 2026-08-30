/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/conformance_profile.c
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
#include "umicom/ui/reactive/conformance_profile.h"
/* Construct the strict cross-frontend reactive-state conformance profile. */
UmiUiReactiveConformanceProfile umi_ui_reactive_conformance_profile_strict(void){UmiUiReactiveConformanceProfile p={true,true,true,true,true,true};return p;}
/* Require all mandatory reactive capabilities for production conformance. */
int umi_ui_reactive_conformance_profile_complete(const UmiUiReactiveConformanceProfile *p){return p&&p->two_way_binding&&p->validation&&p->computed_state&&p->cycle_detection&&p->transactional_updates&&p->renderer_neutral;}
