/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_aliases/layout_save.h
 *
 * PURPOSE:
 *   Publish Studio compatibility command alias layout.save.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ALIAS_LAYOUT_SAVE_H
#define UMICOM_STUDIO_RUNTIME_ALIAS_LAYOUT_SAVE_H
#include "umicom/studio_runtime/command_alias.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Write studio command alias layout in its stable representation and report capacity or
 * input failures to the caller.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_layout_save(void);
#ifdef __cplusplus
}
#endif
#endif
