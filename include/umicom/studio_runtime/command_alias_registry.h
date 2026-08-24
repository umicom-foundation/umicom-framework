/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_alias_registry.h
 *
 * PURPOSE:
 *   Register compatibility aliases into UmiCommandRegistry without overriding
 *   existing commands, and execute their forwarding/surface activation behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_REGISTRY_H
#define UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_REGISTRY_H

#include "umicom/studio_runtime/command_alias_catalogue.h"
#include "umicom/studio_runtime/surface_activator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeCommandAliasRegistry
    UmiStudioRuntimeCommandAliasRegistry;

UmiStatus umi_studio_command_alias_registry_create(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeCommandAliasRegistry **out_registry);

void umi_studio_command_alias_registry_destroy(
    UmiStudioRuntimeCommandAliasRegistry *registry);

UmiStatus umi_studio_command_alias_registry_install(
    UmiStudioRuntimeCommandAliasRegistry *registry);

size_t umi_studio_command_alias_registry_registered_count(
    const UmiStudioRuntimeCommandAliasRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
