/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_aliases/view_terminal.h
 *
 * PURPOSE:
 *   Publish Studio compatibility command alias view.terminal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ALIAS_VIEW_TERMINAL_H
#define UMICOM_STUDIO_RUNTIME_ALIAS_VIEW_TERMINAL_H
#include "umicom/studio_runtime/command_alias.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio command alias view terminal operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_terminal(void);
#ifdef __cplusplus
}
#endif
#endif
