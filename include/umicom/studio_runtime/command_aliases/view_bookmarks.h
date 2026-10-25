/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_aliases/view_bookmarks.h
 *
 * PURPOSE:
 *   Publish Studio compatibility command alias view.bookmarks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ALIAS_VIEW_BOOKMARKS_H
#define UMICOM_STUDIO_RUNTIME_ALIAS_VIEW_BOOKMARKS_H
#include "umicom/studio_runtime/command_alias.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio command alias view bookmark navigation operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *umi_studio_command_alias_view_bookmarks(void);
#ifdef __cplusplus
}
#endif
#endif
