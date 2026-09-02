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

/**
 * Represent the application presentation surface controller binding data shared with
 * callers of this public contract.
 */
typedef struct UmiApplicationPresentationSurfaceControllerBinding {
    const char *component_id;
    UmiApplicationPresentationSurfaceController controller;
    void *context;
} UmiApplicationPresentationSurfaceControllerBinding;

/**
 * Represent the application presentation surface controller registry data shared with
 * callers of this public contract.
 */
typedef struct UmiApplicationPresentationSurfaceControllerRegistry {
    UmiApplicationPresentationSurfaceControllerBinding
        bindings[UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY];
    size_t count;
} UmiApplicationPresentationSurfaceControllerRegistry;

/**
 * Initialise application presentation surface controller registry from caller-provided
 * values so later operations receive a known state.
 */
void umi_application_presentation_surface_controller_registry_init(
    UmiApplicationPresentationSurfaceControllerRegistry *registry);
/**
 * Add application presentation surface controller only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_application_presentation_surface_controller_register(
    UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const char *component_id,
    UmiApplicationPresentationSurfaceController controller,
    void *context);
/**
 * Find application presentation surface controller while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiApplicationPresentationSurfaceControllerBinding *
umi_application_presentation_surface_controller_find(
    const UmiApplicationPresentationSurfaceControllerRegistry *registry,
    const char *component_id);
/**
 * Perform application presentation surface controller through the module contract so
 * client applications do not duplicate its policy.
 */
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
