/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/adoption.h
 *
 * PURPOSE:
 *   Define the minimal product-module contribution that proves a thin
 *   application adopts canonical Framework composition and surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_ADOPTION_H
#define UMICOM_APPLICATION_PRODUCTISATION_ADOPTION_H

#include "umicom/application/productisation/surface_projection.h"
#include "umicom/application/runtime/module_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiProductFrontendFlags {
    UMI_PRODUCT_FRONTEND_CONSOLE = 1U << 0,
    UMI_PRODUCT_FRONTEND_GTK4 = 1U << 1,
    UMI_PRODUCT_FRONTEND_QT6 = 1U << 2,
    UMI_PRODUCT_FRONTEND_WEB = 1U << 3,
    UMI_PRODUCT_FRONTEND_HEADLESS = 1U << 4
} UmiProductFrontendFlags;

typedef struct UmiProductApplicationAdoption {
    uint32_t structure_size;
    const char *module_id;
    const char *application_id;
    const char *display_name;
    const char *executable_id;
    uint32_t frontend_flags;
    int manifest_available;
    int composition_available;
    int executable_available;
    int tests_available;
} UmiProductApplicationAdoption;

typedef struct UmiProductApplicationAdoptionSnapshot {
    uint32_t structure_size;
    char module_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char display_name[UMI_PRODUCTISATION_ID_CAPACITY];
    char executable_id[UMI_PRODUCTISATION_ID_CAPACITY];
    uint32_t frontend_flags;
    UmiApplicationModuleStatus module_status;
    size_t feature_count;
    size_t panel_count;
    size_t layout_count;
    size_t covered_surface_count;
    size_t missing_surface_count;
    int manifest_available;
    int canonical_experience_available;
    int surface_complete;
    int runnable;
    int acceptance_ready;
} UmiProductApplicationAdoptionSnapshot;

UmiStatus umi_product_application_adoption_validate(
    const UmiProductApplicationAdoption *adoption);
UmiStatus umi_product_application_adoption_snapshot(
    const UmiProductApplicationAdoption *adoption,
    UmiProductApplicationAdoptionSnapshot *out_snapshot);
int umi_product_application_adoption_snapshot_accepted(
    const UmiProductApplicationAdoptionSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif
