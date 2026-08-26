/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/readiness.h
 *
 * PURPOSE:
 *   Compute detailed feature readiness and priority backlog summaries for an application experience.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_READINESS_H
#define UMICOM_APPLICATION_RUNTIME_READINESS_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

UmiStatus umi_application_readiness_report(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationReadinessReport *out_report);
int umi_application_readiness_has_open_priority(
    const UmiApplicationReadinessReport *report);

#ifdef __cplusplus
}
#endif

#endif
