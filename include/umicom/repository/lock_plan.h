/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/lock_plan.h
 *
 * PURPOSE:
 *   Build an explicit lock plan before any parent index mutation occurs.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_LOCK_PLAN_H
#define UMICOM_REPOSITORY_LOCK_PLAN_H
#include "umicom/repository/head_set.h"
#include "umicom/repository/inventory.h"
#include "umicom/repository/lock_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiRepositoryLockAction {
    UMI_REPOSITORY_LOCK_NO_CHANGE = 0,
    UMI_REPOSITORY_LOCK_STAGE_GITLINK = 1,
    UMI_REPOSITORY_LOCK_MISSING_HEAD = 2
} UmiRepositoryLockAction;
typedef struct UmiRepositoryLockStep {
    char name[UMI_REPOSITORY_CONTROL_NAME_CAPACITY];
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char head[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
    UmiRepositoryLockAction action;
    int required;
} UmiRepositoryLockStep;
typedef struct UmiRepositoryLockPlan {
    UmiRepositoryLockStep steps[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY];
    size_t count;
    size_t stage_count;
    size_t missing_count;
    int dry_run;
    uint64_t revision;
} UmiRepositoryLockPlan;
UmiStatus umi_repository_lock_plan_build(
    const UmiRepositoryInventory *inventory,
    const UmiRepositoryHeadSet *heads,
    const UmiRepositoryLockPolicy *policy,
    UmiRepositoryLockPlan *out_plan);
const char *umi_repository_lock_action_text(UmiRepositoryLockAction action);
#ifdef __cplusplus
}
#endif
#endif
