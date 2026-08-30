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

static UmiStatus default_controller(
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update)
{
    const char *component_id;
    if (placement == NULL || placement->panel == NULL || out_update == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    component_id = placement->panel->component_id;
    (void)memset(out_update, 0, sizeof(*out_update));
    out_update->state = event == UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT ||
                                event == UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE
        ? UMI_APPLICATION_PRESENTATION_STATE_DORMANT
        : UMI_APPLICATION_PRESENTATION_STATE_READY;
    (void)snprintf(out_update->message, sizeof(out_update->message),
                   "%s handled %s%s%s", component_id,
                   umi_application_presentation_surface_event_text(event),
                   payload != NULL && payload[0] != '\0' ? ": " : "",
                   payload != NULL ? payload : "");
    return UMI_STATUS_OK;
}

void umi_application_presentation_surface_controller_registry_init(
    UmiApplicationPresentationSurfaceControllerRegistry *registry)
{
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

UmiStatus umi_application_presentation_surface_controller_register(
    UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const char *component_id,
    UmiApplicationPresentationSurfaceController controller,
    void *context)
{
    size_t index;
    if (registry == NULL || component_id == NULL || component_id[0] == '\0' ||
        controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->bindings[index].component_id, component_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (registry->count >= UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->bindings[registry->count++] =
        (UmiApplicationPresentationSurfaceControllerBinding){
            component_id, controller, context};
    return UMI_STATUS_OK;
}

const UmiApplicationPresentationSurfaceControllerBinding *
umi_application_presentation_surface_controller_find(
    const UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const char *component_id)
{
    size_t index;
    if (registry == NULL || component_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->bindings[index].component_id, component_id) == 0) {
            return &registry->bindings[index];
        }
    }
    return NULL;
}

UmiStatus umi_application_presentation_surface_controller_dispatch(
    const UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update)
{
    const UmiApplicationPresentationSurfaceControllerBinding *binding;
    if (registry == NULL || placement == NULL || placement->panel == NULL ||
        out_update == NULL ||
        event < UMI_APPLICATION_PRESENTATION_EVENT_MOUNT ||
        event > UMI_APPLICATION_PRESENTATION_EVENT_CONTEXT_CHANGED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    binding = umi_application_presentation_surface_controller_find(
        registry, placement->panel->component_id);
    if (binding == NULL) {
        return default_controller(placement, event, payload, out_update);
    }
    (void)memset(out_update, 0, sizeof(*out_update));
    return binding->controller(binding->context, placement, event, payload,
                               out_update);
}
