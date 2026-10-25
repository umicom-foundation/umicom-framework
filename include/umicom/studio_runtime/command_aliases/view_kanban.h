/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_aliases/view_kanban.h
 *
 * PURPOSE:
 *   Publish Studio compatibility command alias view.kanban.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ALIAS_VIEW_KANBAN_H
#define UMICOM_STUDIO_RUNTIME_ALIAS_VIEW_KANBAN_H
#include "umicom/studio_runtime/command_alias.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio command alias view project board operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *umi_studio_command_alias_view_kanban(void);
#ifdef __cplusplus
}
#endif
#endif
