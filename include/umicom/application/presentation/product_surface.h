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

typedef struct UmiApplicationPresentationProductSurfaceConfig {
    const char *application_id;
    const char *recipe_id;
    UmiApplicationPresentationControllerRegistrar register_controllers;
    void *controller_context;
} UmiApplicationPresentationProductSurfaceConfig;

typedef struct UmiApplicationPresentationProductSurface {
    UmiApplicationPresentationSurfaceRuntime runtime;
    UmiApplicationPresentationHeadlessSurfaceHost headless;
    const char *application_id;
    const char *recipe_id;
    UmiApplicationComponentRecipeAudience audience;
    int initialized;
} UmiApplicationPresentationProductSurface;

UmiStatus umi_application_presentation_product_surface_init(
    const UmiApplicationPresentationProductSurfaceConfig *config,
    UmiApplicationPresentationProductSurface *out_surface);
UmiStatus umi_application_presentation_product_surface_init_for_audience(
    const char *application_id,
    UmiApplicationComponentRecipeAudience audience,
    UmiApplicationPresentationControllerRegistrar register_controllers,
    void *controller_context,
    UmiApplicationPresentationProductSurface *out_surface);
void umi_application_presentation_product_surface_dispose(
    UmiApplicationPresentationProductSurface *surface);

UmiStatus umi_application_presentation_product_surface_activate(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id);
UmiStatus umi_application_presentation_product_surface_deactivate(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id);
UmiStatus umi_application_presentation_product_surface_focus(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id);
UmiStatus umi_application_presentation_product_surface_refresh(
    UmiApplicationPresentationProductSurface *surface);
UmiStatus umi_application_presentation_product_surface_command(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id,
    const char *command_id);
UmiStatus umi_application_presentation_product_surface_context_changed(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id,
    const char *context_value);
UmiStatus umi_application_presentation_product_surface_advance(
    UmiApplicationPresentationProductSurface *surface,
    uint32_t elapsed_seconds);
UmiStatus umi_application_presentation_product_surface_set_background(
    UmiApplicationPresentationProductSurface *surface,
    int background);
UmiStatus umi_application_presentation_product_surface_snapshot(
    const UmiApplicationPresentationProductSurface *surface,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot);

UmiApplicationPresentationSurfaceRuntime *
umi_application_presentation_product_surface_runtime(
    UmiApplicationPresentationProductSurface *surface);
const UmiApplicationPresentationSurfaceRuntime *
umi_application_presentation_product_surface_runtime_const(
    const UmiApplicationPresentationProductSurface *surface);

#ifdef __cplusplus
}
#endif

#endif
