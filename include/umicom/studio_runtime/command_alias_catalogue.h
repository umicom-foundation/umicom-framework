/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_alias_catalogue.h
 *
 * PURPOSE:
 *   Publish all Studio Application Shell compatibility aliases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_CATALOGUE_H
#define UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_CATALOGUE_H
#include "umicom/studio_runtime/command_aliases/aliases.h"
#ifdef __cplusplus
extern "C" {
#endif
size_t umi_studio_command_alias_count(void);
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_at(size_t index);
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_find(const char *alias_id);
#ifdef __cplusplus
}
#endif
#endif
