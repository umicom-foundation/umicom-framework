/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_types.c
 * PURPOSE: Verify stable role, action and operation state contracts.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/helix/execution_types.h"
int main(void)
{
    assert(strcmp(umi_helix_agent_role_text(UMI_HELIX_AGENT_ROLE_REVIEWER),
                  "reviewer") == 0);
    assert(umi_helix_action_kind_is_mutating(UMI_HELIX_ACTION_FILESYSTEM));
    assert(!umi_helix_action_kind_is_mutating(UMI_HELIX_ACTION_REVIEW));
    assert(umi_helix_operation_state_can_transition(
        UMI_HELIX_OPERATION_PLANNED, UMI_HELIX_OPERATION_AWAITING_APPROVAL));
    assert(!umi_helix_operation_state_can_transition(
        UMI_HELIX_OPERATION_DRAFT, UMI_HELIX_OPERATION_RUNNING));
    return 0;
}
