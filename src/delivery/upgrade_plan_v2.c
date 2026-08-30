/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/upgrade_plan_v2.c
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
#include "umicom/delivery/upgrade_plan_v2.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_upgrade_plan_v2_init(UmiUpgradePlanV2 *plan,
                                    const char *current_version,
                                    const char *target_version,
                                    uint64_t current_generation,
                                    uint64_t target_generation,
                                    int compatible)
{
    UmiStatus status;
    if (plan == NULL || current_version == NULL || target_version == NULL ||
        current_generation == 0U || target_generation == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->current_version,
                                    sizeof(plan->current_version), current_version);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->target_version,
                                    sizeof(plan->target_version), target_version);
    if (status != UMI_STATUS_OK) return status;
    plan->current_generation = current_generation;
    plan->target_generation = target_generation;
    plan->compatible = compatible != 0;
    plan->backup_required = 1;
    plan->rollback_supported = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_upgrade_plan_v2_authorise(UmiUpgradePlanV2 *plan,
                                         int backup_available)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!plan->compatible || plan->target_generation <= plan->current_generation) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (plan->backup_required && !backup_available) {
        return UMI_STATUS_UNAVAILABLE;
    }
    plan->authorised = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_upgrade_plan_v2_validate(const UmiUpgradePlanV2 *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->current_version[0] == '\0' || plan->target_version[0] == '\0' ||
        strcmp(plan->current_version, plan->target_version) == 0 ||
        plan->target_generation <= plan->current_generation ||
        !plan->compatible || !plan->rollback_supported || !plan->authorised) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

uint64_t umi_upgrade_plan_v2_rollback_generation(const UmiUpgradePlanV2 *plan)
{
    if (plan == NULL || !plan->rollback_supported) return 0U;
    return plan->current_generation;
}
