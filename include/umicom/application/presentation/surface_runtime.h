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

UmiStatus umi_application_presentation_surface_runtime_init(
    const char *recipe_id,
    UmiApplicationPresentationSurfaceRuntime *out_runtime);
UmiStatus umi_application_presentation_surface_runtime_bind_host(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const UmiApplicationPresentationSurfaceHost *host);
UmiStatus umi_application_presentation_surface_runtime_register_controller(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    UmiApplicationPresentationSurfaceController controller,
    void *context);
UmiStatus umi_application_presentation_surface_runtime_start(
    UmiApplicationPresentationSurfaceRuntime *runtime);
UmiStatus umi_application_presentation_surface_runtime_stop(
    UmiApplicationPresentationSurfaceRuntime *runtime);
UmiStatus umi_application_presentation_surface_runtime_activate(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id);
UmiStatus umi_application_presentation_surface_runtime_deactivate(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id);
UmiStatus umi_application_presentation_surface_runtime_focus(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id);
UmiStatus umi_application_presentation_surface_runtime_refresh(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id);
UmiStatus umi_application_presentation_surface_runtime_refresh_all(
    UmiApplicationPresentationSurfaceRuntime *runtime);
UmiStatus umi_application_presentation_surface_runtime_command(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    const char *command_id);
UmiStatus umi_application_presentation_surface_runtime_context_changed(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    const char *context_value);
UmiStatus umi_application_presentation_surface_runtime_advance(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    uint32_t elapsed_seconds);
UmiStatus umi_application_presentation_surface_runtime_set_background(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    int background);
int umi_application_presentation_surface_runtime_checkpoint_due(
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    uint32_t elapsed_since_checkpoint_seconds,
    int changed);
UmiStatus umi_application_presentation_surface_runtime_restore(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const UmiApplicationPresentationSurfaceCheckpoint *checkpoint);
UmiStatus umi_application_presentation_surface_runtime_snapshot(
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
