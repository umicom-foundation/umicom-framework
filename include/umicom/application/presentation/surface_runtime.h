/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/surface_runtime.h
 *
 * PURPOSE:
 *   Coordinate projected recipes, live panel sessions, product controllers,
 *   frontend hosts, checkpoints and diagnostic events as one reusable runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_SURFACE_RUNTIME_H
#define UMICOM_APPLICATION_PRESENTATION_SURFACE_RUNTIME_H

#include <stdint.h>

#include "umicom/application/presentation/surface_checkpoint.h"
#include "umicom/application/presentation/surface_controller.h"
#include "umicom/application/presentation/surface_host.h"
#include "umicom/application/presentation/surface_journal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application presentation surface runtime data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationPresentationSurfaceRuntime {
    UmiApplicationPresentationSurfaceSession session;
    UmiApplicationPresentationSurfaceControllerRegistry controllers;
    UmiApplicationPresentationSurfaceHost host;
    UmiApplicationPresentationSurfaceJournal journal;
    int host_bound;
    int started;
    int background;
    uint32_t background_elapsed_seconds;
} UmiApplicationPresentationSurfaceRuntime;

/**
 * Initialise application presentation surface runtime from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_presentation_surface_runtime_init(
    const char *recipe_id,
    UmiApplicationPresentationSurfaceRuntime *out_runtime);
/**
 * Provide the application presentation surface runtime bind host operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_bind_host(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const UmiApplicationPresentationSurfaceHost *host);
/**
 * Provide the application presentation surface runtime register controller operation used
 * by this module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_register_controller(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    UmiApplicationPresentationSurfaceController controller,
    void *context);
/**
 * Provide the application presentation surface runtime register controller for all
 * operation used by this module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_register_controller_for_all(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationPresentationSurfaceController controller,
    void *context);
/**
 * Provide the application presentation surface runtime start operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_start(
    UmiApplicationPresentationSurfaceRuntime *runtime);
/**
 * Provide the application presentation surface runtime stop operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_stop(
    UmiApplicationPresentationSurfaceRuntime *runtime);
/**
 * Provide the application presentation surface runtime activate operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_activate(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id);
/**
 * Provide the application presentation surface runtime deactivate operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_deactivate(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id);
/**
 * Provide the application presentation surface runtime focus operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_focus(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id);
/**
 * Provide the application presentation surface runtime refresh operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_refresh(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id);
/**
 * Provide the application presentation surface runtime refresh all operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_refresh_all(
    UmiApplicationPresentationSurfaceRuntime *runtime);
/**
 * Provide the application presentation surface runtime command operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_command(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    const char *command_id);
/**
 * Provide the application presentation surface runtime context changed operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_context_changed(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    const char *context_value);
/**
 * Provide the application presentation surface runtime advance operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_advance(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    uint32_t elapsed_seconds);
/**
 * Provide the application presentation surface runtime set background operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_set_background(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    int background);
/**
 * Provide the application presentation surface runtime checkpoint due operation used by
 * this module and its client applications.
 */
int umi_application_presentation_surface_runtime_checkpoint_due(
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    uint32_t elapsed_since_checkpoint_seconds,
    int changed);
/**
 * Provide the application presentation surface runtime restore operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_restore(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const UmiApplicationPresentationSurfaceCheckpoint *checkpoint);
/**
 * Provide the application presentation surface runtime snapshot operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_snapshot(
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
