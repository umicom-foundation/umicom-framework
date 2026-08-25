/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_docking_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for dock zones, floating, auto-hide and split/tab workstation requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/docking_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcDockingContract x={3U,31U,true}; CHECK(umi_fc_docking_contract_validate(&x));
    return 0;
}
