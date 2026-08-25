/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_drag_drop_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for drag/drop parity scoring with keyboard-accessible fallback requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/drag_drop_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcDragDropEquivalence x={7U,3U,true}; CHECK(umi_fc_drag_drop_equivalence_evaluate(&x)>0.0); CHECK(umi_fc_drag_drop_equivalence_outcome(&x)==UMI_FC_DEGRADED);
    return 0;
}
