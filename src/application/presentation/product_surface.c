/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/product_surface.c
 *
 * PURPOSE:
 *   Implement the shared product-surface lifecycle so applications only add
 *   their identity and small product controllers instead of copying runtime
 *   allocation, host binding and event forwarding code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/product_surface.h"

#include <string.h>

#include "umicom/application/component/recipe_catalogue.h"

static int has_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

static int surface_ready(
    const UmiApplicationPresentationProductSurface *surface)
{
    return surface != NULL && surface->initialized;
}

static UmiStatus surface_status(
    const UmiApplicationPresentationProductSurface *surface)
{
    if (surface == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return surface->initialized ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_application_presentation_product_surface_init(
    const UmiApplicationPresentationProductSurfaceConfig *config,
    UmiApplicationPresentationProductSurface *out_surface)
{
    const UmiApplicationComponentRecipe *recipe;
    UmiStatus status;

    if (config == NULL || out_surface == NULL ||
        !has_text(config->application_id) || !has_text(config->recipe_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_surface, 0, sizeof(*out_surface));
    status = umi_application_presentation_surface_runtime_init(
        config->recipe_id, &out_surface->runtime);
    if (status != UMI_STATUS_OK) return status;

    recipe = out_surface->runtime.session.plan.recipe;
    if (recipe == NULL || recipe->application_id == NULL ||
        strcmp(recipe->application_id, config->application_id) != 0) {
        (void)memset(out_surface, 0, sizeof(*out_surface));
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_surface->application_id = recipe->application_id;
    out_surface->recipe_id = recipe->recipe_id;
    out_surface->audience = recipe->audience;
    umi_application_presentation_headless_surface_host_init(
        &out_surface->headless);
    status = umi_application_presentation_surface_runtime_bind_host(
        &out_surface->runtime,
        umi_application_presentation_headless_surface_host_interface(
            &out_surface->headless));
    if (status == UMI_STATUS_OK && config->register_controllers != NULL) {
        status = config->register_controllers(
            &out_surface->runtime, config->controller_context);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_application_presentation_surface_runtime_start(
            &out_surface->runtime);
    }
    if (status != UMI_STATUS_OK) {
        umi_application_presentation_product_surface_dispose(out_surface);
        return status;
    }
    out_surface->initialized = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_presentation_product_surface_init_for_audience(
    const char *application_id,
    UmiApplicationComponentRecipeAudience audience,
    UmiApplicationPresentationControllerRegistrar register_controllers,
    void *controller_context,
    UmiApplicationPresentationProductSurface *out_surface)
{
    const UmiApplicationComponentRecipe *recipe;
    UmiApplicationPresentationProductSurfaceConfig config;

    if (!has_text(application_id) || out_surface == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    recipe = umi_application_component_recipe_catalogue_recommend(
        application_id, audience);
    if (recipe == NULL) return UMI_STATUS_NOT_FOUND;

    config.application_id = application_id;
    config.recipe_id = recipe->recipe_id;
    config.register_controllers = register_controllers;
    config.controller_context = controller_context;
    return umi_application_presentation_product_surface_init(
        &config, out_surface);
}

void umi_application_presentation_product_surface_dispose(
    UmiApplicationPresentationProductSurface *surface)
{
    if (surface == NULL) return;
    (void)umi_application_presentation_surface_runtime_stop(&surface->runtime);
    (void)memset(surface, 0, sizeof(*surface));
}

UmiStatus umi_application_presentation_product_surface_activate(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_activate(
              &surface->runtime, component_id)
        : status;
}

UmiStatus umi_application_presentation_product_surface_deactivate(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_deactivate(
              &surface->runtime, component_id)
        : status;
}

UmiStatus umi_application_presentation_product_surface_focus(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_focus(
              &surface->runtime, component_id)
        : status;
}

UmiStatus umi_application_presentation_product_surface_refresh(
    UmiApplicationPresentationProductSurface *surface)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_refresh_all(
              &surface->runtime)
        : status;
}

UmiStatus umi_application_presentation_product_surface_command(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id,
    const char *command_id)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_command(
              &surface->runtime, component_id, command_id)
        : status;
}

UmiStatus umi_application_presentation_product_surface_context_changed(
    UmiApplicationPresentationProductSurface *surface,
    const char *component_id,
    const char *context_value)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_context_changed(
              &surface->runtime, component_id, context_value)
        : status;
}

UmiStatus umi_application_presentation_product_surface_advance(
    UmiApplicationPresentationProductSurface *surface,
    uint32_t elapsed_seconds)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_advance(
              &surface->runtime, elapsed_seconds)
        : status;
}

UmiStatus umi_application_presentation_product_surface_set_background(
    UmiApplicationPresentationProductSurface *surface,
    int background)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_set_background(
              &surface->runtime, background)
        : status;
}

UmiStatus umi_application_presentation_product_surface_snapshot(
    const UmiApplicationPresentationProductSurface *surface,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_snapshot(
              &surface->runtime, out_snapshot)
        : status;
}

UmiApplicationPresentationSurfaceRuntime *
umi_application_presentation_product_surface_runtime(
    UmiApplicationPresentationProductSurface *surface)
{
    return surface_ready(surface) ? &surface->runtime : NULL;
}

const UmiApplicationPresentationSurfaceRuntime *
umi_application_presentation_product_surface_runtime_const(
    const UmiApplicationPresentationProductSurface *surface)
{
    return surface_ready(surface) ? &surface->runtime : NULL;
}
