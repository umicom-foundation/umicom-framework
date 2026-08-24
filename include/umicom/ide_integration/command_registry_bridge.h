/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/command_registry_bridge.h
 *
 * PURPOSE:
 *   Register cross-domain IDE integration commands into the authoritative
 *   Framework UmiCommandRegistry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_COMMAND_REGISTRY_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_COMMAND_REGISTRY_BRIDGE_H

#include "umicom/ide_integration/command_router.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeCommandRegistryBridge UmiIdeCommandRegistryBridge;

UmiStatus umi_ide_command_registry_bridge_create(
    UmiCommandRegistry *registry,
    UmiIdeIntegrationPlatform *platform,
    UmiIdeCommandRegistryBridge **out_bridge);

void umi_ide_command_registry_bridge_destroy(
    UmiIdeCommandRegistryBridge *bridge);

UmiStatus umi_ide_command_registry_bridge_set_context(
    UmiIdeCommandRegistryBridge *bridge,
    const UmiIdeCommandContext *context);

UmiStatus umi_ide_command_registry_bridge_register(
    UmiIdeCommandRegistryBridge *bridge);

#ifdef __cplusplus
}
#endif
#endif
