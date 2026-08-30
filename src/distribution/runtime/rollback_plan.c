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

void umi_dr_rollback_plan_init(UmiDrRollbackPlan *value) { if (value != NULL) { *value = (UmiDrRollbackPlan){0};  } }
bool umi_dr_rollback_plan_valid(const UmiDrRollbackPlan *value) { return value != NULL && (value->id[0] != '\0' && value->checkpoint_id[0] != '\0' && value->verified); }
uint64_t umi_dr_rollback_plan_fingerprint(const UmiDrRollbackPlan *value) {
    uint64_t h = 0U;
    if (!umi_dr_rollback_plan_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
