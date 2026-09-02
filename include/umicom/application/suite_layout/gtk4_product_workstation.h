/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/gtk4_product_workstation.h
 *
 * PURPOSE:
 *   Provide one reusable native product workstation which joins canonical
 *   application layouts, live presentation controllers and GTK4 rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_PRODUCT_WORKSTATION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_PRODUCT_WORKSTATION_H

#include <gtk/gtk.h>

#include "umicom/application/presentation/product_panel_projection.h"
#include "umicom/application/presentation/product_surface.h"
#include "umicom/application/suite_layout/gtk4_workstation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_PRODUCT_GTK4_WORKSTATION_API_VERSION 1U

/** Values required to compose one thin application as a native workstation. */
typedef struct UmiApplicationProductGtk4WorkstationConfig {
    const char *application_id;
    const char *title;
    const char *mode_badge;
    UmiApplicationComponentRecipeAudience audience;
    UmiApplicationPresentationControllerRegistrar register_controllers;
    void *controller_context;
} UmiApplicationProductGtk4WorkstationConfig;

/** Copied product, layout and panel-coverage state for tests and diagnostics. */
typedef struct UmiApplicationProductGtk4WorkstationSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiApplicationSuiteGtk4WorkstationSnapshot layout;
    UmiApplicationPresentationSurfaceSnapshot surface;
    UmiApplicationProductPanelCoverage coverage;
    uint64_t revision;
} UmiApplicationProductGtk4WorkstationSnapshot;

/**
 * Represent the application product gtk4 workstation data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductGtk4Workstation
    UmiApplicationProductGtk4Workstation;

/**
 * Return safe defaults for a standard native product workstation.
 *
 * The returned value borrows all supplied strings and callbacks. The create
 * function copies identity text and retains callback pointers for its lifetime.
 */
UmiApplicationProductGtk4WorkstationConfig
umi_application_product_gtk4_workstation_config_default(
    const char *application_id,
    const char *title,
    UmiApplicationPresentationControllerRegistrar register_controllers,
    void *controller_context);

/** Create layout, product-surface, controller and rendering services together. */
UmiStatus umi_application_product_gtk4_workstation_create(
    const UmiApplicationProductGtk4WorkstationConfig *config,
    UmiApplicationProductGtk4Workstation **out_workstation);
/** Release widgets, presentation state and all owned memory. */
void umi_application_product_gtk4_workstation_destroy(
    UmiApplicationProductGtk4Workstation *workstation);
/** Borrow the root widget for placement inside a native application window. */
GtkWidget *umi_application_product_gtk4_workstation_widget(
    UmiApplicationProductGtk4Workstation *workstation);
/** Select and render a canonical product layout by stable identifier. */
UmiStatus umi_application_product_gtk4_workstation_select_layout(
    UmiApplicationProductGtk4Workstation *workstation,
    const char *layout_id);
/** Select a Framework appearance profile without application-owned CSS. */
UmiStatus umi_application_product_gtk4_workstation_select_appearance(
    UmiApplicationProductGtk4Workstation *workstation,
    const char *profile_id);
/** Refresh controller state and rebuild visible panel projections. */
UmiStatus umi_application_product_gtk4_workstation_refresh(
    UmiApplicationProductGtk4Workstation *workstation);
/** Begin a reversible layout editing transaction. */
UmiStatus umi_application_product_gtk4_workstation_begin_layout_edit(
    UmiApplicationProductGtk4Workstation *workstation);
/** Keep valid layout edits and end the active transaction. */
UmiStatus umi_application_product_gtk4_workstation_commit_layout_edit(
    UmiApplicationProductGtk4Workstation *workstation);
/** Discard layout edits and restore the transaction's starting arrangement. */
UmiStatus umi_application_product_gtk4_workstation_cancel_layout_edit(
    UmiApplicationProductGtk4Workstation *workstation);
/** Copy current product, rendering and coverage facts without exposing widgets. */
UmiStatus umi_application_product_gtk4_workstation_snapshot(
    const UmiApplicationProductGtk4Workstation *workstation,
    UmiApplicationProductGtk4WorkstationSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
