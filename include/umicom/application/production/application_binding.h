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

/**
 * Represent the application production binding data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationProductionBinding {
    uint32_t structure_size;
    const UmiProductApplicationAdoption *adoption;
    const UmiApplicationExperienceDefinition *experience;
    UmiProductApplicationAdoptionSnapshot adoption_snapshot;
} UmiApplicationProductionBinding;

/**
 * Initialise application production binding from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_production_binding_init(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationProductionBinding *out_binding);
/**
 * Check that application production binding satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_production_binding_validate(
    const UmiApplicationProductionBinding *binding);

#ifdef __cplusplus
}
#endif
#endif
