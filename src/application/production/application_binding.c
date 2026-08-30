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

UmiStatus umi_application_production_binding_init(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationProductionBinding *out_binding)
{
    UmiStatus status;
    if (adoption == NULL || out_binding == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_binding, 0, sizeof(*out_binding));
    status = umi_product_application_adoption_validate(adoption);
    if (status != UMI_STATUS_OK) return status;
    out_binding->structure_size = sizeof(*out_binding);
    out_binding->adoption = adoption;
    out_binding->experience = umi_application_experience_catalogue_find(
        adoption->application_id);
    if (out_binding->experience == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_product_application_adoption_snapshot(
        adoption, &out_binding->adoption_snapshot);
    if (status != UMI_STATUS_OK) {
        (void)memset(out_binding, 0, sizeof(*out_binding));
        return status;
    }
    return umi_application_production_binding_validate(out_binding);
}

UmiStatus umi_application_production_binding_validate(
    const UmiApplicationProductionBinding *binding)
{
    if (binding == NULL || binding->structure_size != sizeof(*binding) ||
        binding->adoption == NULL || binding->experience == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(binding->adoption->application_id,
               binding->experience->application_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (binding->adoption_snapshot.application_id[0] == '\0')
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

