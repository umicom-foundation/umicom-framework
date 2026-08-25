/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_drag_drop_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for semantic drag/drop operation, keyboard alternative and docking affordance requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/drag_drop_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcDragDropContract x={3U,true,true}; CHECK(umi_fc_drag_drop_contract_validate(&x));
    return 0;
}
