/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/surface_profiles/source_control.h
 *
 * PURPOSE:
 *   Publish the Source Control logical IDE surface profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_SURFACE_PROFILE_SOURCE_CONTROL_H
#define UMICOM_IDE_SURFACE_PROFILE_SOURCE_CONTROL_H
#include "umicom/ide_integration/surface_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide surface profile source control operation used by this module and its
 * client applications.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_source_control(void);
#ifdef __cplusplus
}
#endif
#endif
