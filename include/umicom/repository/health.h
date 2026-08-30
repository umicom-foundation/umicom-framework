/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/health.h
 *
 * PURPOSE:
 *   Summarize repository dependency and lock-plan health.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_HEALTH_H
#define UMICOM_REPOSITORY_HEALTH_H
#include "umicom/repository/lock_plan.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryHealth {
    size_t dependency_count;
    size_t stage_count;
    size_t missing_count;
    size_t warning_count;
    size_t error_count;
    int healthy;
} UmiRepositoryHealth;
UmiStatus umi_repository_health_from_plan(
    const UmiRepositoryLockPlan *plan,
    UmiRepositoryHealth *out_health);
#ifdef __cplusplus
}
#endif
#endif
