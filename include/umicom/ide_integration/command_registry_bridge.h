/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/command_registry_bridge.h
 *
 * PURPOSE:
 *   Register cross-domain IDE integration commands into the authoritative
 *   Framework UmiCommandRegistry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_COMMAND_REGISTRY_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_COMMAND_REGISTRY_BRIDGE_H

#include "umicom/ide_integration/command_router.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ide command registry bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiIdeCommandRegistryBridge UmiIdeCommandRegistryBridge;

/**
 * Initialise ide command registry bridge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ide_command_registry_bridge_create(
    UmiCommandRegistry *registry,
    UmiIdeIntegrationPlatform *platform,
    UmiIdeCommandRegistryBridge **out_bridge);

/**
 * Release or reset state held by ide command registry bridge so the same storage can be
 * reused safely.
 */
void umi_ide_command_registry_bridge_destroy(
    UmiIdeCommandRegistryBridge *bridge);

/**
 * Provide the ide command registry bridge set context operation used by this module and
 * its client applications.
 */
UmiStatus umi_ide_command_registry_bridge_set_context(
    UmiIdeCommandRegistryBridge *bridge,
    const UmiIdeCommandContext *context);

/**
 * Add ide command registry bridge only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ide_command_registry_bridge_register(
    UmiIdeCommandRegistryBridge *bridge);

#ifdef __cplusplus
}
#endif
#endif
