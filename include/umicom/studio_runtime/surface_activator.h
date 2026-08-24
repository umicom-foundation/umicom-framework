/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_activator.h
 *
 * PURPOSE:
 *   Apply resolved command activation to the existing Application Shell state
 *   and semantic layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_ACTIVATOR_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_ACTIVATOR_H

#include "umicom/studio_runtime/activation_catalogue.h"
#include "umicom/studio_runtime/bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_surface_activate_kind(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeSurfaceKind kind,
    int make_visible,
    int focus);

UmiStatus umi_studio_surface_activate_for_command(
    UmiStudioRuntimeBindings *bindings,
    const char *command_id);

#ifdef __cplusplus
}
#endif
#endif
