/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/builtin_surfaces.h
 *
 * PURPOSE:
 *   Install all Framework IDE logical surfaces into one registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_BUILTIN_SURFACES_H
#define UMICOM_IDE_INTEGRATION_BUILTIN_SURFACES_H
#include "umicom/ide_integration/surface_profiles/profiles.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_builtin_surfaces_install(UmiIdeSurfaceRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
