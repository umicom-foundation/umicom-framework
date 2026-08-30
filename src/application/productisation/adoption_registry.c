/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/adoption_registry.c
 *
 * PURPOSE:
 *   Implement bounded registration and evidence summaries for independently
 *   versioned thin application contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/adoption_registry.h"

#include <string.h>

void umi_product_adoption_registry_init(
    UmiProductAdoptionRegistry *registry)
{
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

const UmiProductApplicationAdoption *umi_product_adoption_registry_find(
    const UmiProductAdoptionRegistry *registry,
    const char *application_id)
{
    size_t index;
    if (registry == NULL || application_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->entries[index]->application_id,
                   application_id) == 0)
            return registry->entries[index];
    }
    return NULL;
}

UmiStatus umi_product_adoption_registry_register(
    UmiProductAdoptionRegistry *registry,
    const UmiProductApplicationAdoption *adoption)
{
    size_t index;
    UmiStatus status;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_application_adoption_validate(adoption);
    if (status != UMI_STATUS_OK) return status;
    if (umi_product_adoption_registry_find(
            registry, adoption->application_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->entries[index]->module_id,
                   adoption->module_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_PRODUCTISATION_MAX_APPLICATIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->entries[registry->count++] = adoption;
    return UMI_STATUS_OK;
}

const UmiProductApplicationAdoption *umi_product_adoption_registry_at(
    const UmiProductAdoptionRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? registry->entries[index] : NULL;
}

UmiStatus umi_product_adoption_registry_report(
    const UmiProductAdoptionRegistry *registry,
    UmiProductAdoptionRegistryReport *out_report)
{
    size_t index;
    if (registry == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->contribution_count = registry->count;
    for (index = 0U; index < registry->count; ++index) {
        UmiProductApplicationAdoptionSnapshot snapshot;
        const UmiStatus status = umi_product_application_adoption_snapshot(
            registry->entries[index], &snapshot);
        if (status != UMI_STATUS_OK) {
            out_report->invalid_count += 1U;
            continue;
        }
        if (snapshot.canonical_experience_available)
            out_report->canonical_count += 1U;
        if (snapshot.runnable) out_report->runnable_count += 1U;
        if (snapshot.module_status.tests_available)
            out_report->tested_count += 1U;
        if (snapshot.layout_projection_complete)
            out_report->layout_ready_count += 1U;
        if (snapshot.surface_complete)
            out_report->surface_complete_count += 1U;
        if (snapshot.acceptance_ready)
            out_report->accepted_count += 1U;
    }
    return UMI_STATUS_OK;
}
