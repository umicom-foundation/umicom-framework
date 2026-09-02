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

/* Provide the has text operation used by this module and its client applications. */
static int has_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

/* Provide the surface ready operation used by this module and its client applications. */
static int surface_ready(
    const UmiApplicationPresentationProductSurface *surface)
{
    return surface != NULL && surface->initialized;
}

/* Provide the surface status operation used by this module and its client applications. */
static UmiStatus surface_status(
    const UmiApplicationPresentationProductSurface *surface)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return surface->initialized ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/*
 * Initialise application presentation product surface from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_presentation_product_surface_init(
    const UmiApplicationPresentationProductSurfaceConfig *config,
    UmiApplicationPresentationProductSurface *out_surface)
{
    const UmiApplicationComponentRecipe *recipe;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || out_surface == NULL ||
        !has_text(config->application_id) || !has_text(config->recipe_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_surface, 0, sizeof(*out_surface));
    status = umi_application_presentation_surface_runtime_init(
        config->recipe_id, &out_surface->runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    recipe = out_surface->runtime.session.plan.recipe;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && config->register_controllers != NULL) {
        status = config->register_controllers(
            &out_surface->runtime, config->controller_context);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_presentation_surface_runtime_start(
            &out_surface->runtime);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_application_presentation_product_surface_dispose(out_surface);
        return status;
    }
    out_surface->initialized = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the application presentation product surface init for audience operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_init_for_audience(
    const char *application_id,
    UmiApplicationComponentRecipeAudience audience,
    UmiApplicationPresentationControllerRegistrar register_controllers,
    void *controller_context,
    UmiApplicationPresentationProductSurface *out_surface)
{
    const UmiApplicationComponentRecipe *recipe;
    UmiApplicationPresentationProductSurfaceConfig config;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!has_text(application_id) || out_surface == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    recipe = umi_application_component_recipe_catalogue_recommend(
        application_id, audience);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (recipe == NULL) return UMI_STATUS_NOT_FOUND;

    config.application_id = application_id;
    config.recipe_id = recipe->recipe_id;
    config.register_controllers = register_controllers;
    config.controller_context = controller_context;
    return umi_application_presentation_product_surface_init(
        &config, out_surface);
}

/*
 * Release or reset state held by application presentation product surface so the same
 * storage can be reused safely.
 */
void umi_application_presentation_product_surface_dispose(
    UmiApplicationPresentationProductSurface *surface)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL) return;
    (void)umi_application_presentation_surface_runtime_stop(&surface->runtime);
    (void)memset(surface, 0, sizeof(*surface));
}

/*
 * Provide the application presentation product surface activate operation used by this
 * module and its client applications.
 */
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

/*
 * Provide the application presentation product surface deactivate operation used by this
 * module and its client applications.
 */
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

/*
 * Provide the application presentation product surface focus operation used by this module
 * and its client applications.
 */
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

/*
 * Provide the application presentation product surface refresh operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_product_surface_refresh(
    UmiApplicationPresentationProductSurface *surface)
{
    UmiStatus status = surface_status(surface);
    return status == UMI_STATUS_OK
        ? umi_application_presentation_surface_runtime_refresh_all(
              &surface->runtime)
        : status;
}

/*
 * Provide the application presentation product surface command operation used by this
 * module and its client applications.
 */
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

/*
 * Provide the application presentation product surface context changed operation used by
 * this module and its client applications.
 */
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

/*
 * Provide the application presentation product surface advance operation used by this
 * module and its client applications.
 */
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

/*
 * Provide the application presentation product surface set background operation used by
 * this module and its client applications.
 */
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

/*
 * Provide the application presentation product surface snapshot operation used by this
 * module and its client applications.
 */
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

/*
 * Provide the application presentation product surface runtime operation used by this
 * module and its client applications.
 */
UmiApplicationPresentationSurfaceRuntime *
umi_application_presentation_product_surface_runtime(
    UmiApplicationPresentationProductSurface *surface)
{
    return surface_ready(surface) ? &surface->runtime : NULL;
}

/*
 * Provide the application presentation product surface runtime const operation used by
 * this module and its client applications.
 */
const UmiApplicationPresentationSurfaceRuntime *
umi_application_presentation_product_surface_runtime_const(
    const UmiApplicationPresentationProductSurface *surface)
{
    return surface_ready(surface) ? &surface->runtime : NULL;
}
