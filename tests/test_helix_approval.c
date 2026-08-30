/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_approval.c
 * PURPOSE: Verify exact plan/action human approval binding.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/approval_gate.h"
int main(void)
{
    UmiHelixApprovalGate gate;
    umi_helix_approval_gate_init(&gate);
    assert(umi_helix_approval_gate_request(&gate, "op", "write", "abc")
           == UMI_STATUS_OK);
    assert(!umi_helix_approval_gate_is_approved(&gate, "op", "write", "abc"));
    assert(umi_helix_approval_gate_decide(
        &gate, "op", "write", UMI_HELIX_APPROVAL_APPROVED,
        "sammy", "Reviewed exact plan") == UMI_STATUS_OK);
    assert(umi_helix_approval_gate_is_approved(&gate, "op", "write", "abc"));
    assert(!umi_helix_approval_gate_is_approved(&gate, "op", "write", "changed"));
    return 0;
}
