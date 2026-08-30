/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/evidence_requirement.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_EVIDENCE_REQUIREMENT_H
#define UMICOM_APPLICATION_PRODUCTION_EVIDENCE_REQUIREMENT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"

typedef struct UmiApplicationProductionEvidenceRequirement {
    char evidence_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    UmiApplicationProductionEvidenceKind kind;
    int required;
} UmiApplicationProductionEvidenceRequirement;

typedef struct UmiApplicationProductionEvidenceRequirements {
    UmiApplicationProductionEvidenceRequirement
        entries[UMI_APPLICATION_PRODUCTION_MAX_EVIDENCE];
    size_t count;
    size_t required_count;
} UmiApplicationProductionEvidenceRequirements;

UmiStatus umi_application_production_evidence_requirements_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionEvidenceRequirements *out_requirements);
const UmiApplicationProductionEvidenceRequirement *
umi_application_production_evidence_requirements_find(
    const UmiApplicationProductionEvidenceRequirements *requirements,
    const char *evidence_id);

#ifdef __cplusplus
}
#endif
#endif
