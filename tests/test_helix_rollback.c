/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_rollback.c
 * PURPOSE: Verify approved reverse-order compensating actions.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/helix/compensation_plan.h"
int main(void)
{
    UmiHelixCompensationPlan plan;
    UmiHelixAction first;
    UmiHelixAction second;
    umi_helix_compensation_plan_init(&plan);
    assert(umi_helix_action_init(&first, "undo-a", UMI_HELIX_ACTION_FILESYSTEM,
        "Undo A", "a.c", 1U) == UMI_STATUS_OK);
    assert(umi_helix_action_init(&second, "undo-b", UMI_HELIX_ACTION_FILESYSTEM,
        "Undo B", "b.c", 1U) == UMI_STATUS_OK);
    assert(umi_helix_compensation_plan_add(&plan, &first) == UMI_STATUS_OK);
    assert(umi_helix_compensation_plan_add(&plan, &second) == UMI_STATUS_OK);
    assert(umi_helix_compensation_plan_begin(&plan, 1) == UMI_STATUS_OK);
    assert(strcmp(umi_helix_compensation_plan_next(&plan)->action_id, "undo-b") == 0);
    assert(umi_helix_compensation_plan_record(&plan, UMI_STATUS_OK) == UMI_STATUS_OK);
    assert(strcmp(umi_helix_compensation_plan_next(&plan)->action_id, "undo-a") == 0);
    assert(umi_helix_compensation_plan_record(&plan, UMI_STATUS_OK) == UMI_STATUS_OK);
    assert(plan.state == UMI_HELIX_ROLLBACK_COMPLETE);
    return 0;
}
