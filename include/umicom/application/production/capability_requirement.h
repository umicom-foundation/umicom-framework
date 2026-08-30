/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/capability_requirement.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_CAPABILITY_REQUIREMENT_H
#define UMICOM_APPLICATION_PRODUCTION_CAPABILITY_REQUIREMENT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"

typedef struct UmiApplicationProductionCapabilityRequirement {
    char capability_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    size_t panel_consumers;
    size_t feature_consumers;
    size_t critical_consumers;
} UmiApplicationProductionCapabilityRequirement;

typedef struct UmiApplicationProductionCapabilityRequirements {
    UmiApplicationProductionCapabilityRequirement
        entries[UMI_APPLICATION_PRODUCTION_MAX_CAPABILITIES];
    size_t count;
    size_t critical_count;
} UmiApplicationProductionCapabilityRequirements;

UmiStatus umi_application_production_capability_requirements_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionCapabilityRequirements *out_requirements);
const UmiApplicationProductionCapabilityRequirement *
umi_application_production_capability_requirements_find(
    const UmiApplicationProductionCapabilityRequirements *requirements,
    const char *capability_id);

#ifdef __cplusplus
}
#endif
#endif
