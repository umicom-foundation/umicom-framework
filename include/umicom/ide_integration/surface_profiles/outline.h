/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/surface_profiles/outline.h
 *
 * PURPOSE:
 *   Publish the Outline logical IDE surface profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_SURFACE_PROFILE_OUTLINE_H
#define UMICOM_IDE_SURFACE_PROFILE_OUTLINE_H
#include "umicom/ide_integration/surface_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide surface profile outline operation used by this module and its client
 * applications.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_outline(void);
#ifdef __cplusplus
}
#endif
#endif
