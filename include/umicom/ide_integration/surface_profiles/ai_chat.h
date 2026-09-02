/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/surface_profiles/ai_chat.h
 *
 * PURPOSE:
 *   Publish the AI Coding Chat logical IDE surface profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_SURFACE_PROFILE_AI_CHAT_H
#define UMICOM_IDE_SURFACE_PROFILE_AI_CHAT_H
#include "umicom/ide_integration/surface_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide surface profile ai chat operation used by this module and its client
 * applications.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_ai_chat(void);
#ifdef __cplusplus
}
#endif
#endif
