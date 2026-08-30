/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/surface_profile.h
 *
 * PURPOSE:
 *   Define the factory signature used by independent IDE surface profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_SURFACE_PROFILE_H
#define UMICOM_IDE_INTEGRATION_SURFACE_PROFILE_H
#include "umicom/ide_integration/surface.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef const UmiIdeSurfaceDescriptor *(*UmiIdeSurfaceProfileFactory)(void);
#ifdef __cplusplus
}
#endif
#endif
