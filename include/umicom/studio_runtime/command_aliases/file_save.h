/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_aliases/file_save.h
 *
 * PURPOSE:
 *   Publish Studio compatibility command alias file.save.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ALIAS_FILE_SAVE_H
#define UMICOM_STUDIO_RUNTIME_ALIAS_FILE_SAVE_H
#include "umicom/studio_runtime/command_alias.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Write studio command alias file in its stable representation and report capacity or
 * input failures to the caller.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_file_save(void);
#ifdef __cplusplus
}
#endif
#endif
