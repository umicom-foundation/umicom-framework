/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_degradation_record.c
 *
 * PURPOSE:
 *   Focused regression coverage for explicit record of reduced frontend behaviour and user-visible consequences.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/degradation_record.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcDegradationRecord r; CHECK(umi_fc_degradation_record_make("dock",1U,UMI_FC_BLOCKER,"missing",&r)==UMI_STATUS_OK); CHECK(umi_fc_degradation_record_is_blocker(&r));
    return 0;
}
