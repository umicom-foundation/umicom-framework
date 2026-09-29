/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: examples/release_baseline/client/main.c
 * Purpose: Consume only the installed public evidence and distribution contracts.
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/evidence.h"
int main(void)
{
    UmiDrReleaseGateInput input = {true,true,true,true,true,0U};
    if (!umi_dr_release_gate_pass(&input)) return 1;
    input.blockers = 1U;
    if (umi_dr_release_gate_pass(&input)) return 2;
    UmiReleaseContract *contract = NULL;
    if (UmiReleaseContractParse("invalid",7U,&contract) != UMI_STATUS_PARSE_ERROR || contract != NULL) return 3;
    if (UmiReleaseContractCount(NULL) != 0U || UmiReleaseContractAt(NULL,0U) != NULL) return 4;
    return 0;
}
