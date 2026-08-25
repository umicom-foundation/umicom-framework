/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_command_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for command-surface parity scoring and missing-command blocker reporting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/command_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcCommandEquivalence x={10U,9U,0U}; CHECK(umi_fc_command_equivalence_evaluate(&x)>0.89); CHECK(umi_fc_command_equivalence_passes(&x,0.85)); x.blockers=1U; CHECK(!umi_fc_command_equivalence_passes(&x,0.85));
    return 0;
}
