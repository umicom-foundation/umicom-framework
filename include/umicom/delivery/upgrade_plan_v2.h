/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/upgrade_plan_v2.h
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
#ifndef UMICOM_DELIVERY_UPGRADE_PLAN_V2_H
#define UMICOM_DELIVERY_UPGRADE_PLAN_V2_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUpgradePlanV2 {
    char current_version[UMI_DELIVERY_VERSION_CAPACITY];
    char target_version[UMI_DELIVERY_VERSION_CAPACITY];
    uint64_t current_generation;
    uint64_t target_generation;
    int compatible;
    int backup_required;
    int rollback_supported;
    int authorised;
} UmiUpgradePlanV2;

UmiStatus umi_upgrade_plan_v2_init(UmiUpgradePlanV2 *plan,
                                    const char *current_version,
                                    const char *target_version,
                                    uint64_t current_generation,
                                    uint64_t target_generation,
                                    int compatible);
UmiStatus umi_upgrade_plan_v2_authorise(UmiUpgradePlanV2 *plan,
                                         int backup_available);
UmiStatus umi_upgrade_plan_v2_validate(const UmiUpgradePlanV2 *plan);
uint64_t umi_upgrade_plan_v2_rollback_generation(const UmiUpgradePlanV2 *plan);

#ifdef __cplusplus
}
#endif
#endif
