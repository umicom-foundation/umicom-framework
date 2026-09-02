/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_activator.h
 *
 * PURPOSE:
 *   Apply resolved command activation to the existing Application Shell state
 *   and semantic layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_ACTIVATOR_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_ACTIVATOR_H

#include "umicom/studio_runtime/activation_catalogue.h"
#include "umicom/studio_runtime/bindings.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the studio surface activate kind operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_surface_activate_kind(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeSurfaceKind kind,
    int make_visible,
    int focus);

/**
 * Provide the studio surface activate for command operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_surface_activate_for_command(
    UmiStudioRuntimeBindings *bindings,
    const char *command_id);

#ifdef __cplusplus
}
#endif
#endif
