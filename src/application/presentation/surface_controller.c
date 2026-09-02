/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/surface_controller.c
 *
 * PURPOSE:
 *   Dispatch presentation events to optional product controllers and provide
 *   safe default behavior for fully declarative reusable panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/surface_controller.h"

#include <stdio.h>
#include <string.h>

#include "umicom/application/presentation/surface_behavior_catalogue.h"

/*
 * Provide the default controller operation used by this module and its client
 * applications.
 */
static UmiStatus default_controller(
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update)
{
    const char *component_id;
    const UmiApplicationPresentationSurfaceBehavior *behavior;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (placement == NULL || placement->panel == NULL || out_update == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    component_id = placement->panel->component_id;
    behavior = umi_application_presentation_surface_behavior_catalogue_find(
        component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (behavior == NULL) return UMI_STATUS_NOT_FOUND;
    (void)memset(out_update, 0, sizeof(*out_update));
    out_update->state = event == UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT ||
                                event == UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE
        ? UMI_APPLICATION_PRESENTATION_STATE_DORMANT
        : behavior->initial_state;
    (void)snprintf(out_update->message, sizeof(out_update->message),
                   "%s handled %s%s%s", component_id,
                   umi_application_presentation_surface_event_text(event),
                   payload != NULL && payload[0] != '\0' ? ": " : "",
                   payload != NULL ? payload : "");
    return UMI_STATUS_OK;
}

/*
 * Initialise application presentation surface controller registry from caller-provided
 * values so later operations receive a known state.
 */
void umi_application_presentation_surface_controller_registry_init(
    UmiApplicationPresentationSurfaceControllerRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

/*
 * Add application presentation surface controller only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_application_presentation_surface_controller_register(
    UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const char *component_id,
    UmiApplicationPresentationSurfaceController controller,
    void *context)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || component_id == NULL || component_id[0] == '\0' ||
        controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->bindings[index].component_id, component_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->bindings[registry->count++] =
        (UmiApplicationPresentationSurfaceControllerBinding){
            component_id, controller, context};
    return UMI_STATUS_OK;
}

/*
 * Find application presentation surface controller while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiApplicationPresentationSurfaceControllerBinding *
umi_application_presentation_surface_controller_find(
    const UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const char *component_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || component_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->bindings[index].component_id, component_id) == 0) {
            return &registry->bindings[index];
        }
    }
    return NULL;
}

/*
 * Perform application presentation surface controller through the module contract so
 * client applications do not duplicate its policy.
 */
UmiStatus umi_application_presentation_surface_controller_dispatch(
    const UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update)
{
    const UmiApplicationPresentationSurfaceControllerBinding *binding;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || placement == NULL || placement->panel == NULL ||
        out_update == NULL ||
        event < UMI_APPLICATION_PRESENTATION_EVENT_MOUNT ||
        event > UMI_APPLICATION_PRESENTATION_EVENT_CONTEXT_CHANGED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    binding = umi_application_presentation_surface_controller_find(
        registry, placement->panel->component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL) {
        return default_controller(placement, event, payload, out_update);
    }
    (void)memset(out_update, 0, sizeof(*out_update));
    return binding->controller(binding->context, placement, event, payload,
                               out_update);
}
