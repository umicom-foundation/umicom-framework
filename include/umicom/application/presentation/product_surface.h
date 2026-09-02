/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/product_surface.h
 *
 * PURPOSE:
 *   Provide one reusable application-surface lifecycle for every Umicom
 *   product, including recipe selection, controller registration, a headless
 *   host and safe access to the shared presentation runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_PRODUCT_SURFACE_H
#define UMICOM_APPLICATION_PRESENTATION_PRODUCT_SURFACE_H

#include "umicom/application/component/recipe.h"
#include "umicom/application/presentation/headless_surface_host.h"
#include "umicom/application/presentation/surface_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiApplicationPresentationControllerRegistrar)(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    void *context);

/**
 * Represent the application presentation product surface config data shared with callers
 * of this public contract.
 */
typedef struct UmiApplicationPresentationProductSurfaceConfig {
    const char *application_id;
    const char *recipe_id;
    UmiApplicationPresentationControllerRegistrar register_controllers;
    void *controller_context;
} UmiApplicationPresentationProductSurfaceConfig;

/**
 * Represent the application presentation product surface data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationPresentationProductSurface {
    UmiApplicationPresentationSurfaceRuntime runtime;
    UmiApplicationPresentationHeadlessSurfaceHost headless;
    const char *application_id;
    const char *recipe_id;
    UmiApplicationComponentRecipeAudience audience;
    int initialized;
} UmiApplicationPresentationProductSurface;

/**
 * Initialise application presentation product surface from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_presentation_product_surface_init(
    const UmiApplicationPresentationProductSurfaceConfig *config,
    UmiApplicationPresentationProductSurface *out_surface);
/**
 * Provide the application presentation product surface init for audience operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_init_for_audience(
    const char *application_id,
    UmiApplicationComponentRecipeAudience audience,
    UmiApplicationPresentationControllerRegistrar register_controllers,
    void *controller_context,
    UmiApplicationPresentationProductSurface *out_surface);
/**
 * Release or reset state held by application presentation product surface so the same
 * storage can be reused safely.
 */
void umi_application_presentation_product_surface_dispose(
    UmiApplicationPresentationProductSurface *surface);

/**
 * Provide the application presentation product surface activate operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_activate(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id);
/**
 * Provide the application presentation product surface deactivate operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_deactivate(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id);
/**
 * Provide the application presentation product surface focus operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_focus(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id);
/**
 * Provide the application presentation product surface refresh operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_refresh(
    UmiApplicationPresentationProductSurface *surface);
/**
 * Provide the application presentation product surface command operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_command(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id,
    const char *command_id);
/**
 * Provide the application presentation product surface context changed operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_context_changed(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id,
    const char *context_value);
/**
 * Provide the application presentation product surface advance operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_advance(
    UmiApplicationPresentationProductSurface *surface,
    uint32_t elapsed_seconds);
/**
 * Provide the application presentation product surface set background operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_set_background(
    UmiApplicationPresentationProductSurface *surface,
    int background);
/**
 * Provide the application presentation product surface snapshot operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_snapshot(
    const UmiApplicationPresentationProductSurface *surface,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot);

/**
 * Provide the application presentation product surface runtime operation used by this
 * module and its client applications.
 */
UmiApplicationPresentationSurfaceRuntime *
umi_application_presentation_product_surface_runtime(
    UmiApplicationPresentationProductSurface *surface);
/**
 * Provide the application presentation product surface runtime const operation used by
 * this module and its client applications.
 */
const UmiApplicationPresentationSurfaceRuntime *
umi_application_presentation_product_surface_runtime_const(
    const UmiApplicationPresentationProductSurface *surface);

#ifdef __cplusplus
}
#endif

#endif
