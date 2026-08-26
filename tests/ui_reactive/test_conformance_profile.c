/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_conformance_profile.c
 *
 * PURPOSE:
 *   Exercise the conformance profile reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/conformance_profile.h"
int main(void) { UmiUiReactiveConformanceProfile p=umi_ui_reactive_conformance_profile_strict();return umi_ui_reactive_conformance_profile_complete(&p)?0:1; }
