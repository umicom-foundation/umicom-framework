/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_workstation_conformance.c
 *
 * PURPOSE:
 *   Focused regression coverage for aggregate workstation parity across layout, interaction, accessibility and rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/workstation_conformance.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcWorkstationConformance w={0.95,0.94,0.98,0.96,0.90,0U}; CHECK(umi_fc_workstation_conformance_score(&w)>0.94); CHECK(umi_fc_workstation_conformance_outcome(&w,0.90)==UMI_FC_PASS);
    return 0;
}
