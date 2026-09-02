/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/command_alias_registry.h
 *
 * PURPOSE:
 *   Register compatibility aliases into UmiCommandRegistry without overriding
 *   existing commands, and execute their forwarding/surface activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_REGISTRY_H
#define UMICOM_STUDIO_RUNTIME_COMMAND_ALIAS_REGISTRY_H

#include "umicom/studio_runtime/command_alias_catalogue.h"
#include "umicom/studio_runtime/surface_activator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime command alias registry data shared with callers of this
 * public contract.
 */
typedef struct UmiStudioRuntimeCommandAliasRegistry
    UmiStudioRuntimeCommandAliasRegistry;

/**
 * Initialise studio command alias registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_command_alias_registry_create(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeCommandAliasRegistry **out_registry);

/**
 * Release or reset state held by studio command alias registry so the same storage can be
 * reused safely.
 */
void umi_studio_command_alias_registry_destroy(
    UmiStudioRuntimeCommandAliasRegistry *registry);

/**
 * Provide the studio command alias registry install operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_command_alias_registry_install(
    UmiStudioRuntimeCommandAliasRegistry *registry);

/**
 * Return the number of records represented by studio command alias registry registered
 * without changing their state.
 */
size_t umi_studio_command_alias_registry_registered_count(
    const UmiStudioRuntimeCommandAliasRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
