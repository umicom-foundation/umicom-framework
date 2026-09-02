/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/readiness.h
 *
 * PURPOSE:
 *   Compute detailed feature readiness and priority backlog summaries for an application experience.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_READINESS_H
#define UMICOM_APPLICATION_RUNTIME_READINESS_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application readiness report data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationReadinessReport {
    uint32_t structure_size;
    size_t feature_count;
    size_t planned_count;
    size_t foundation_count;
    size_t implemented_count;
    size_t verified_count;
    size_t p0_open_count;
    size_t p1_open_count;
    size_t framework_owned_count;
    size_t application_owned_count;
    size_t external_adapter_count;
    unsigned readiness_percent;
    const UmiExperienceFeatureDefinition *next_feature;
} UmiApplicationReadinessReport;

/**
 * Provide the application readiness report operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_readiness_report(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationReadinessReport *out_report);
/**
 * Provide the application readiness has open priority operation used by this module and
 * its client applications.
 */
int umi_application_readiness_has_open_priority(
    const UmiApplicationReadinessReport *report);

#ifdef __cplusplus
}
#endif

#endif
