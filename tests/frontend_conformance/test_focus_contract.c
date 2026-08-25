/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_focus_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for focusable-element ordering and focus-trap requirements for interactive surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/focus_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcFocusContract x={4U,4U,true}; CHECK(umi_fc_focus_contract_validate(&x));
    return 0;
}
