/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_catalogue.h
 *
 * PURPOSE:
 *   Publish the complete Studio surface binding catalogue and install it into
 *   the existing Application Shell registry/layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_CATALOGUE_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_CATALOGUE_H

#include "umicom/studio_runtime/surface_profiles/profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_studio_surface_catalogue_count(void);

const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_at(size_t index);

const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_for_kind(UmiStudioRuntimeSurfaceKind kind);

const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_find_ide_surface(const char *surface_id);

UmiStatus umi_studio_surface_catalogue_install(
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellLayout *layout);

#ifdef __cplusplus
}
#endif
#endif
