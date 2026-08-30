/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/application_binding.h
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
#ifndef UMICOM_APPLICATION_PRODUCTION_APPLICATION_BINDING_H
#define UMICOM_APPLICATION_PRODUCTION_APPLICATION_BINDING_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/productisation/adoption.h"

typedef struct UmiApplicationProductionBinding {
    uint32_t structure_size;
    const UmiProductApplicationAdoption *adoption;
    const UmiApplicationExperienceDefinition *experience;
    UmiProductApplicationAdoptionSnapshot adoption_snapshot;
} UmiApplicationProductionBinding;

UmiStatus umi_application_production_binding_init(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationProductionBinding *out_binding);
UmiStatus umi_application_production_binding_validate(
    const UmiApplicationProductionBinding *binding);

#ifdef __cplusplus
}
#endif
#endif
