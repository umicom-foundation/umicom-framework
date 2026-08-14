/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/upgrade_plan.h
 *
 * PURPOSE:
 *   Plan product upgrades with explicit compatibility, backup and rollback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_UPGRADE_PLAN_H
#define UMICOM_DELIVERY_UPGRADE_PLAN_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUpgradePlan {
    char current_version[UMI_DELIVERY_VERSION_CAPACITY];
    char target_version[UMI_DELIVERY_VERSION_CAPACITY];
    uint64_t current_generation;
    uint64_t target_generation;
    int compatible;
    int backup_required;
    int rollback_supported;
    int authorised;
} UmiUpgradePlan;

UmiStatus umi_upgrade_plan_init(UmiUpgradePlan *plan,
                                    const char *current_version,
                                    const char *target_version,
                                    uint64_t current_generation,
                                    uint64_t target_generation,
                                    int compatible);
UmiStatus umi_upgrade_plan_authorise(UmiUpgradePlan *plan,
                                         int backup_available);
UmiStatus umi_upgrade_plan_validate(const UmiUpgradePlan *plan);
uint64_t umi_upgrade_plan_rollback_generation(const UmiUpgradePlan *plan);

#ifdef __cplusplus
}
#endif
#endif
