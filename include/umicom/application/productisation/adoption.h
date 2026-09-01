/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/adoption.h
 *
 * PURPOSE:
 *   Define the minimal product-module contribution that proves a thin
 *   application adopts canonical Framework composition and surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_ADOPTION_H
#define UMICOM_APPLICATION_PRODUCTISATION_ADOPTION_H

#include "umicom/application/productisation/surface_projection.h"
#include "umicom/application/suite_layout/runtime.h"
#include "umicom/application/runtime/module_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Identify the presentation adapters supplied by one thin application. */
typedef enum UmiProductFrontendFlags {
    UMI_PRODUCT_FRONTEND_FLAG_CONSOLE = 1U << 0,
    UMI_PRODUCT_FRONTEND_FLAG_GTK4 = 1U << 1,
    UMI_PRODUCT_FRONTEND_FLAG_QT6 = 1U << 2,
    UMI_PRODUCT_FRONTEND_FLAG_WEB = 1U << 3,
    UMI_PRODUCT_FRONTEND_FLAG_HEADLESS = 1U << 4
} UmiProductFrontendFlags;

/**
 * Describe the small amount of evidence contributed by an application repo.
 *
 * The Framework looks up application_id in its canonical experience catalogue.
 * The four availability fields report whether the application repo supplied
 * its manifest, composition, executable and tests; they do not duplicate any
 * feature, panel or layout metadata.
 */
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

/**
 * Record the Framework's checked view of one application contribution.
 *
 * Counts come from the canonical experience, projection fields come from the
 * shared layout runtime, and readiness becomes true only when every required
 * piece of evidence is present. Callers receive a value snapshot and own it.
 */
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
    size_t projected_layout_count;
    size_t projected_window_count;
    size_t default_layout_window_count;
    size_t covered_surface_count;
    size_t missing_surface_count;
    int manifest_available;
    int canonical_experience_available;
    int layout_runtime_ready;
    int layout_projection_complete;
    int surface_complete;
    int runnable;
    int acceptance_ready;
} UmiProductApplicationAdoptionSnapshot;

/** Validate identity, frontend and catalogue references in a contribution. */
UmiStatus umi_product_application_adoption_validate(
    const UmiProductApplicationAdoption *adoption);
/** Build a complete readiness snapshot from Framework-owned catalogues. */
UmiStatus umi_product_application_adoption_snapshot(
    const UmiProductApplicationAdoption *adoption,
    UmiProductApplicationAdoptionSnapshot *out_snapshot);
/** Load the application's default canonical layout into a shared runtime. */
UmiStatus umi_product_application_adoption_layout_load(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationSuiteLayoutRuntime *out_runtime);
/** Return non-zero only when the supplied snapshot passed every acceptance check. */
int umi_product_application_adoption_snapshot_accepted(
    const UmiProductApplicationAdoptionSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif
