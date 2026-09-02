/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_catalogue.h
 *
 * PURPOSE:
 *   Publish the complete Studio surface binding catalogue and install it into
 *   the existing Application Shell registry/layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_CATALOGUE_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_CATALOGUE_H

#include "umicom/studio_runtime/surface_profiles/profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by studio surface catalogue without changing
 * their state.
 */
size_t umi_studio_surface_catalogue_count(void);

/**
 * Find studio surface catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_at(size_t index);

/**
 * Provide the studio surface catalogue for kind operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_for_kind(UmiStudioRuntimeSurfaceKind kind);

/**
 * Provide the studio surface catalogue find ide surface operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_find_ide_surface(const char *surface_id);

/**
 * Provide the studio surface catalogue install operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_surface_catalogue_install(
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellLayout *layout);

#ifdef __cplusplus
}
#endif
#endif
