/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/update_plan.h
 *
 * PURPOSE:
 *   Describe one proposed update before any installed files are changed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Planning first allows Studio to show the target generation, restart requirement and reason before applying an update.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_UPDATE_PLAN_H
#define INCLUDE_UMICOM_DELIVERY_UPDATE_PLAN_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUpdatePlan {
    uint64_t current_generation;
    uint64_t target_generation;
    UmiUpdateDecision decision;
    int requires_restart;
    char reason[UMI_DELIVERY_TEXT_CAPACITY];
} UmiUpdatePlan;

UmiStatus umi_update_plan_init(UmiUpdatePlan *plan,
                               uint64_t current_generation,
                               uint64_t target_generation,
                               UmiUpdateDecision decision,
                               const char *reason);

#ifdef __cplusplus
}
#endif

#endif
