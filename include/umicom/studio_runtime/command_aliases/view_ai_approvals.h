/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_aliases/view_ai_approvals.h
 *
 * PURPOSE:
 *   Publish Studio compatibility command alias view.ai-approvals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ALIAS_VIEW_AI_APPROVALS_H
#define UMICOM_STUDIO_RUNTIME_ALIAS_VIEW_AI_APPROVALS_H
#include "umicom/studio_runtime/command_alias.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio command alias view ai approvals operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_ai_approvals(void);
#ifdef __cplusplus
}
#endif
#endif
