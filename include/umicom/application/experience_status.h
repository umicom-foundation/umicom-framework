/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience_status.h
 *
 * PURPOSE:
 *   Summarise implementation readiness for a canonical application experience.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCE_STATUS_H
#define UMICOM_APPLICATION_EXPERIENCE_STATUS_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceStatus {
    uint32_t structure_size;
    size_t panel_count;
    size_t layout_count;
    size_t feature_count;
    size_t planned_count;
    size_t foundation_count;
    size_t implemented_count;
    size_t verified_count;
    unsigned readiness_percent;
} UmiApplicationExperienceStatus;

UmiStatus umi_application_experience_status(
    const UmiApplicationExperienceDefinition *definition,
    UmiApplicationExperienceStatus *out_status);

#ifdef __cplusplus
}
#endif

#endif
