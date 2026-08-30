/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_v2_rollback.c
 * PURPOSE: Verify approved reverse-order compensating actions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/helix/rollback_v2.h"
int main(void)
{
    UmiHelixRollbackPlanV2 plan;
    UmiHelixActionV2 first;
    UmiHelixActionV2 second;
    umi_helix_rollback_v2_init(&plan);
    assert(umi_helix_action_v2_init(&first, "undo-a", UMI_HELIX_ACTION_FILESYSTEM,
        "Undo A", "a.c", 1U) == UMI_STATUS_OK);
    assert(umi_helix_action_v2_init(&second, "undo-b", UMI_HELIX_ACTION_FILESYSTEM,
        "Undo B", "b.c", 1U) == UMI_STATUS_OK);
    assert(umi_helix_rollback_v2_add(&plan, &first) == UMI_STATUS_OK);
    assert(umi_helix_rollback_v2_add(&plan, &second) == UMI_STATUS_OK);
    assert(umi_helix_rollback_v2_begin(&plan, 1) == UMI_STATUS_OK);
    assert(strcmp(umi_helix_rollback_v2_next(&plan)->action_id, "undo-b") == 0);
    assert(umi_helix_rollback_v2_record(&plan, UMI_STATUS_OK) == UMI_STATUS_OK);
    assert(strcmp(umi_helix_rollback_v2_next(&plan)->action_id, "undo-a") == 0);
    assert(umi_helix_rollback_v2_record(&plan, UMI_STATUS_OK) == UMI_STATUS_OK);
    assert(plan.state == UMI_HELIX_ROLLBACK_COMPLETE);
    return 0;
}
