/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_keyboard_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for required command and navigation keyboard coverage for workstation surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/keyboard_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcKeyboardContract x={5U,2U,true}; CHECK(umi_fc_keyboard_contract_validate(&x));
    return 0;
}
