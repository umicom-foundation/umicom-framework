/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/upgrade_plan.c
 *
 * PURPOSE:
 *   Plan product upgrades with explicit compatibility, backup and rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/upgrade_plan.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise upgrade plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_upgrade_plan_init(UmiUpgradePlan *plan,
                                    const char *current_version,
                                    const char *target_version,
                                    uint64_t current_generation,
                                    uint64_t target_generation,
                                    int compatible)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || current_version == NULL || target_version == NULL ||
        current_generation == 0U || target_generation == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->current_version,
                                    sizeof(plan->current_version), current_version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->target_version,
                                    sizeof(plan->target_version), target_version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->current_generation = current_generation;
    plan->target_generation = target_generation;
    plan->compatible = compatible != 0;
    plan->backup_required = 1;
    plan->rollback_supported = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the upgrade plan authorise operation used by this module and its client
 * applications.
 */
UmiStatus umi_upgrade_plan_authorise(UmiUpgradePlan *plan,
                                         int backup_available)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!plan->compatible || plan->target_generation <= plan->current_generation) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->backup_required && !backup_available) {
        return UMI_STATUS_UNAVAILABLE;
    }
    plan->authorised = 1;
    return UMI_STATUS_OK;
}

/* Check that upgrade plan satisfies its contract before another service relies on it. */
UmiStatus umi_upgrade_plan_validate(const UmiUpgradePlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->current_version[0] == '\0' || plan->target_version[0] == '\0' ||
        strcmp(plan->current_version, plan->target_version) == 0 ||
        plan->target_generation <= plan->current_generation ||
        !plan->compatible || !plan->rollback_supported || !plan->authorised) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the upgrade plan rollback generation operation used by this module and its
 * client applications.
 */
uint64_t umi_upgrade_plan_rollback_generation(const UmiUpgradePlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || !plan->rollback_supported) return 0U;
    return plan->current_generation;
}
