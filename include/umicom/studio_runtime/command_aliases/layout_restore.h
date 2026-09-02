/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_aliases/layout_restore.h
 *
 * PURPOSE:
 *   Publish Studio compatibility command alias layout.restore.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ALIAS_LAYOUT_RESTORE_H
#define UMICOM_STUDIO_RUNTIME_ALIAS_LAYOUT_RESTORE_H
#include "umicom/studio_runtime/command_alias.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio command alias layout restore operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_layout_restore(void);
#ifdef __cplusplus
}
#endif
#endif
