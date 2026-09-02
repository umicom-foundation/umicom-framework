/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/surface_profiles/ai_tools.h
 *
 * PURPOSE:
 *   Publish the AI Tool Activity logical IDE surface profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_SURFACE_PROFILE_AI_TOOLS_H
#define UMICOM_IDE_SURFACE_PROFILE_AI_TOOLS_H
#include "umicom/ide_integration/surface_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide surface profile ai tools operation used by this module and its client
 * applications.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_ai_tools(void);
#ifdef __cplusplus
}
#endif
#endif
