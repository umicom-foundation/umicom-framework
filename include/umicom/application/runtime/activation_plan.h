/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/activation_plan.h
 *
 * PURPOSE:
 *   Build capability-aware panel activation plans before a frontend creates views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_ACTIVATION_PLAN_H
#define UMICOM_APPLICATION_RUNTIME_ACTIVATION_PLAN_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application activation entry data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationActivationEntry {
    const UmiExperiencePanelDefinition *panel;
    int capability_available;
    int required;
} UmiApplicationActivationEntry;

/**
 * Represent the application activation plan data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationActivationPlan {
    uint32_t structure_size;
    const UmiExperienceLayoutDefinition *layout;
    UmiApplicationActivationEntry entries[UMI_APPLICATION_RUNTIME_MAX_PANELS];
    size_t entry_count;
    size_t blocked_count;
} UmiApplicationActivationPlan;

/**
 * Provide the application activation plan build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_activation_plan_build(
    const UmiApplicationExperienceDefinition *experience,
    const char *layout_id,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationActivationPlan *out_plan);
/**
 * Provide the application activation plan ready operation used by this module and its
 * client applications.
 */
int umi_application_activation_plan_ready(
    const UmiApplicationActivationPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
