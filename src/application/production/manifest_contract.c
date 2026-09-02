/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/manifest_contract.c
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
#include "umicom/application/production/manifest_contract.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the application production manifest contract build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_manifest_contract_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionManifestContract *out_contract)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_contract == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_contract, 0, sizeof(*out_contract));
    status = copy_text(out_contract->application_id,
        sizeof(out_contract->application_id),
        binding->experience->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_contract->display_name,
        sizeof(out_contract->display_name),
        binding->experience->display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_contract->executable_id,
        sizeof(out_contract->executable_id),
        binding->adoption->executable_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_contract->default_layout_id,
        sizeof(out_contract->default_layout_id),
        binding->experience->default_layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_contract->frontend_flags = binding->adoption->frontend_flags;
    out_contract->panel_count = binding->experience->panel_count;
    out_contract->layout_count = binding->experience->layout_count;
    out_contract->feature_count = binding->experience->feature_count;
    return umi_application_production_manifest_contract_validate(out_contract);
}

/*
 * Check that application production manifest contract satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_application_production_manifest_contract_validate(
    const UmiApplicationProductionManifestContract *contract)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (contract == NULL || contract->application_id[0] == '\0' ||
        contract->display_name[0] == '\0' ||
        contract->executable_id[0] == '\0' ||
        contract->default_layout_id[0] == '\0' ||
        contract->frontend_flags == 0U || contract->panel_count == 0U ||
        contract->layout_count == 0U || contract->feature_count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

