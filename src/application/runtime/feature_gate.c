/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/feature_gate.c
 *
 * PURPOSE:
 *   Implement feature gates with explicit state and capability evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/feature_gate.h"

#include <string.h>

/*
 * Provide the application feature gate evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_feature_gate_evaluate(
    const UmiApplicationExperienceDefinition *experience,
    const char *feature_id,
    UmiExperienceFeatureState minimum_state,
    UmiApplicationCapabilityProbe probe,
    void *user_data,
    UmiApplicationFeatureGateResult *out_result)
{
    const UmiExperienceFeatureDefinition *feature;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || feature_id == NULL || out_result == NULL ||
        minimum_state < UMI_EXPERIENCE_FEATURE_PLANNED ||
        minimum_state > UMI_EXPERIENCE_FEATURE_VERIFIED)
        return UMI_STATUS_INVALID_ARGUMENT;
    feature = umi_application_experience_feature_find(experience, feature_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (feature == NULL) return UMI_STATUS_NOT_FOUND;
    memset(out_result, 0, sizeof(*out_result));
    out_result->structure_size = sizeof(*out_result);
    out_result->feature = feature;
    out_result->minimum_state = minimum_state;
    out_result->state_satisfied = feature->state >= minimum_state;
    out_result->capability_satisfied = feature->required_capability == NULL ||
        feature->required_capability[0] == '\0' || probe == NULL ||
        probe(feature->required_capability, user_data) != 0;
    out_result->allowed = out_result->state_satisfied && out_result->capability_satisfied;
    return UMI_STATUS_OK;
}
