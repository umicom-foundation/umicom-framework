/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience_plan.h
 *
 * PURPOSE:
 *   Expose deterministic priority and next-feature queries for product implementation planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCE_PLAN_H
#define UMICOM_APPLICATION_EXPERIENCE_PLAN_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by application experience priority without
 * changing their state.
 */
size_t umi_application_experience_priority_count(
    const UmiApplicationExperienceDefinition *definition,
    UmiExperiencePriority priority);

/**
 * Provide the application experience next feature operation used by this module and its
 * client applications.
 */
const UmiExperienceFeatureDefinition *umi_application_experience_next_feature(
    const UmiApplicationExperienceDefinition *definition);

/**
 * Provide the application experience has unverified critical feature operation used by
 * this module and its client applications.
 */
int umi_application_experience_has_unverified_critical_feature(
    const UmiApplicationExperienceDefinition *definition);

#ifdef __cplusplus
}
#endif

#endif
