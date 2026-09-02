/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/builtin_surfaces.h
 *
 * PURPOSE:
 *   Install all Framework IDE logical surfaces into one registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_BUILTIN_SURFACES_H
#define UMICOM_IDE_INTEGRATION_BUILTIN_SURFACES_H
#include "umicom/ide_integration/surface_profiles/profiles.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide builtin surfaces install operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_builtin_surfaces_install(UmiIdeSurfaceRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
