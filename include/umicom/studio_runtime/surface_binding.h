/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_binding.h
 *
 * PURPOSE:
 *   Map logical IDE surfaces onto Application Shell contributions without
 *   duplicating existing shell panels. A preferred contribution identity is
 *   used first; title/role/region matching provides a compatibility fallback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_BINDING_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_BINDING_H

#include "umicom/application_shell/registry.h"
#include "umicom/application_shell/layout.h"
#include "umicom/ide_integration/surface.h"
#include "umicom/studio_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeSurfaceBinding {
    UmiStudioRuntimeSurfaceKind kind;
    const char *ide_surface_id;
    const char *preferred_contribution_id;
    const char *fallback_contribution_id;
    const char *activity_id;
    const char *container_id;
    const char *title;
    UmiApplicationShellRole role;
    UmiApplicationShellRegion region;
    int32_t order;
    int default_visible;
    int create_if_missing;
} UmiStudioRuntimeSurfaceBinding;

UmiStatus umi_studio_runtime_surface_binding_validate(
    const UmiStudioRuntimeSurfaceBinding *binding);

UmiStatus umi_studio_runtime_surface_resolve(
    UmiApplicationShellRegistry *registry,
    const UmiStudioRuntimeSurfaceBinding *binding,
    UmiApplicationShellContribution *out_contribution);

UmiStatus umi_studio_runtime_surface_install(
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellLayout *layout,
    const UmiStudioRuntimeSurfaceBinding *binding);

#ifdef __cplusplus
}
#endif
#endif
