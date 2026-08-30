/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/surface_controller.h
 *
 * PURPOSE:
 *   Register small product controllers against reusable Framework component
 *   identifiers and dispatch lifecycle, refresh and command events to them.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_SURFACE_CONTROLLER_H
#define UMICOM_APPLICATION_PRESENTATION_SURFACE_CONTROLLER_H

#include "umicom/application/presentation/runtime_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiApplicationPresentationSurfaceController)(
    void *context,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update);

typedef struct UmiApplicationPresentationSurfaceControllerBinding {
    const char *component_id;
    UmiApplicationPresentationSurfaceController controller;
    void *context;
} UmiApplicationPresentationSurfaceControllerBinding;

typedef struct UmiApplicationPresentationSurfaceControllerRegistry {
    UmiApplicationPresentationSurfaceControllerBinding
        bindings[UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY];
    size_t count;
} UmiApplicationPresentationSurfaceControllerRegistry;

void umi_application_presentation_surface_controller_registry_init(
    UmiApplicationPresentationSurfaceControllerRegistry *registry);
UmiStatus umi_application_presentation_surface_controller_register(
    UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const char *component_id,
    UmiApplicationPresentationSurfaceController controller,
    void *context);
const UmiApplicationPresentationSurfaceControllerBinding *
umi_application_presentation_surface_controller_find(
    const UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const char *component_id);
UmiStatus umi_application_presentation_surface_controller_dispatch(
    const UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update);

#ifdef __cplusplus
}
#endif

#endif
