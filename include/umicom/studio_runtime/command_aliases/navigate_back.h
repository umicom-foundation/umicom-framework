/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_aliases/navigate_back.h
 *
 * PURPOSE:
 *   Publish Studio compatibility command alias navigate.back.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ALIAS_NAVIGATE_BACK_H
#define UMICOM_STUDIO_RUNTIME_ALIAS_NAVIGATE_BACK_H
#include "umicom/studio_runtime/command_alias.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio command alias navigate back operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_navigate_back(void);
#ifdef __cplusplus
}
#endif
#endif
