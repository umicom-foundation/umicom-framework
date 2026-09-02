/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/rollback_plan.c
 *
 * PURPOSE:
 *   rollback checkpoint and prior-version restoration policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/rollback_plan.h"

/*
 * Initialise dr rollback plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_rollback_plan_init(UmiDrRollbackPlan *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrRollbackPlan){0};  } }
/* Check that dr rollback plan satisfies its contract before another service relies on it. */
bool umi_dr_rollback_plan_valid(const UmiDrRollbackPlan *value) { return value != NULL && (value->id[0] != '\0' && value->checkpoint_id[0] != '\0' && value->verified); }
/*
 * Provide the dr rollback plan fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_rollback_plan_fingerprint(const UmiDrRollbackPlan *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_rollback_plan_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
