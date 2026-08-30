/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_conformance_profile.c
 *
 * PURPOSE:
 *   Exercise the conformance profile reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/conformance_profile.h"
int main(void) { UmiUiReactiveConformanceProfile p=umi_ui_reactive_conformance_profile_strict();return umi_ui_reactive_conformance_profile_complete(&p)?0:1; }
