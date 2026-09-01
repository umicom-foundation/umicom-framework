/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/adoption_registry.h
 *
 * PURPOSE:
 *   Collect thin application adoption contributions and report portfolio
 *   module, executable, test and surface readiness without owning products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_ADOPTION_REGISTRY_H
#define UMICOM_APPLICATION_PRODUCTISATION_ADOPTION_REGISTRY_H

#include "umicom/application/productisation/adoption.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Hold borrowed pointers to validated application contributions. */
typedef struct UmiProductAdoptionRegistry {
    const UmiProductApplicationAdoption
        *entries[UMI_PRODUCTISATION_MAX_APPLICATIONS];
    size_t count;
} UmiProductAdoptionRegistry;

/** Summarise how many registered applications passed each readiness stage. */
typedef struct UmiProductAdoptionRegistryReport {
    size_t contribution_count;
    size_t canonical_count;
    size_t runnable_count;
    size_t tested_count;
    size_t layout_ready_count;
    size_t surface_complete_count;
    size_t accepted_count;
    size_t invalid_count;
} UmiProductAdoptionRegistryReport;

/** Clear a registry before its first contribution is registered. */
void umi_product_adoption_registry_init(
    UmiProductAdoptionRegistry *registry);
/** Register one borrowed contribution after validation and duplicate checks. */
UmiStatus umi_product_adoption_registry_register(
    UmiProductAdoptionRegistry *registry,
    const UmiProductApplicationAdoption *adoption);
/** Borrow the contribution at index, or return NULL when index is out of range. */
const UmiProductApplicationAdoption *umi_product_adoption_registry_at(
    const UmiProductAdoptionRegistry *registry,
    size_t index);
/** Borrow the contribution with an exact application ID match. */
const UmiProductApplicationAdoption *umi_product_adoption_registry_find(
    const UmiProductAdoptionRegistry *registry,
    const char *application_id);
/** Re-evaluate registered contributions and write aggregate readiness counts. */
UmiStatus umi_product_adoption_registry_report(
    const UmiProductAdoptionRegistry *registry,
    UmiProductAdoptionRegistryReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
