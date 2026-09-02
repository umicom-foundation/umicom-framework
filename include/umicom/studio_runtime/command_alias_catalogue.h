/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_alias_catalogue.h
 *
 * PURPOSE:
 *   Publish all Studio Application Shell compatibility aliases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_CATALOGUE_H
#define UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_CATALOGUE_H
#include "umicom/studio_runtime/command_aliases/aliases.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Return the number of records represented by studio command alias without changing their
 * state.
 */
size_t umi_studio_command_alias_count(void);
/**
 * Find studio command alias while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_at(size_t index);
/**
 * Find studio command alias while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_find(const char *alias_id);
#ifdef __cplusplus
}
#endif
#endif
