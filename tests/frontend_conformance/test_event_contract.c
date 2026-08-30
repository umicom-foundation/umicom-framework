/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_event_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for semantic user-event support requirements independent of native toolkit event classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/event_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcEventContract x={3U,true,true}; CHECK(umi_fc_event_contract_validate(&x));
    return 0;
}
