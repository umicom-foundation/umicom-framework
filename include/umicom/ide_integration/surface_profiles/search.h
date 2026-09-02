/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/surface_profiles/search.h
 *
 * PURPOSE:
 *   Publish the Search logical IDE surface profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_SURFACE_PROFILE_SEARCH_H
#define UMICOM_IDE_SURFACE_PROFILE_SEARCH_H
#include "umicom/ide_integration/surface_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide surface profile search operation used by this module and its client
 * applications.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_search(void);
#ifdef __cplusplus
}
#endif
#endif
