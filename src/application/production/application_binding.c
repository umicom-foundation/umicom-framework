/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/application_binding.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/application_binding.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"

/*
 * Initialise application production binding from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_production_binding_init(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationProductionBinding *out_binding)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adoption == NULL || out_binding == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_binding, 0, sizeof(*out_binding));
    status = umi_product_application_adoption_validate(adoption);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_binding->structure_size = sizeof(*out_binding);
    out_binding->adoption = adoption;
    out_binding->experience = umi_application_experience_catalogue_find(
        adoption->application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_binding->experience == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_product_application_adoption_snapshot(
        adoption, &out_binding->adoption_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(out_binding, 0, sizeof(*out_binding));
        return status;
    }
    return umi_application_production_binding_validate(out_binding);
}

/*
 * Check that application production binding satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_production_binding_validate(
    const UmiApplicationProductionBinding *binding)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL || binding->structure_size != sizeof(*binding) ||
        binding->adoption == NULL || binding->experience == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(binding->adoption->application_id,
               binding->experience->application_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (binding->adoption_snapshot.application_id[0] == '\0')
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

