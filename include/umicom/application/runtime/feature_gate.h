/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/feature_gate.h
 *
 * PURPOSE:
 *   Evaluate feature maturity and required Framework capability before a thin product exposes an action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_FEATURE_GATE_H
#define UMICOM_APPLICATION_RUNTIME_FEATURE_GATE_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationFeatureGateResult {
    uint32_t structure_size;
    const UmiExperienceFeatureDefinition *feature;
    UmiExperienceFeatureState minimum_state;
    int state_satisfied;
    int capability_satisfied;
    int allowed;
} UmiApplicationFeatureGateResult;

UmiStatus umi_application_feature_gate_evaluate(
    const UmiApplicationExperienceDefinition *experience,
    const char *feature_id,
    UmiExperienceFeatureState minimum_state,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationFeatureGateResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
