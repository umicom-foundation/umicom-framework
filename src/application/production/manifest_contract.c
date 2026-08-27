/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/manifest_contract.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/manifest_contract.h"

#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_application_production_manifest_contract_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionManifestContract *out_contract)
{
    UmiStatus status;
    if (out_contract == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_contract, 0, sizeof(*out_contract));
    status = copy_text(out_contract->application_id,
        sizeof(out_contract->application_id),
        binding->experience->application_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_contract->display_name,
        sizeof(out_contract->display_name),
        binding->experience->display_name);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_contract->executable_id,
        sizeof(out_contract->executable_id),
        binding->adoption->executable_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_contract->default_layout_id,
        sizeof(out_contract->default_layout_id),
        binding->experience->default_layout_id);
    if (status != UMI_STATUS_OK) return status;
    out_contract->frontend_flags = binding->adoption->frontend_flags;
    out_contract->panel_count = binding->experience->panel_count;
    out_contract->layout_count = binding->experience->layout_count;
    out_contract->feature_count = binding->experience->feature_count;
    return umi_application_production_manifest_contract_validate(out_contract);
}

UmiStatus umi_application_production_manifest_contract_validate(
    const UmiApplicationProductionManifestContract *contract)
{
    if (contract == NULL || contract->application_id[0] == '\0' ||
        contract->display_name[0] == '\0' ||
        contract->executable_id[0] == '\0' ||
        contract->default_layout_id[0] == '\0' ||
        contract->frontend_flags == 0U || contract->panel_count == 0U ||
        contract->layout_count == 0U || contract->feature_count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

