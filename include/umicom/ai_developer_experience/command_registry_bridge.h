/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/command_registry_bridge.h
 *
 * PURPOSE:
 *   Register AI Developer Experience commands in the Framework's authoritative
 *   runtime command registry so menus, toolbars, palettes, automation and AI can
 *   discover the same command surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_REGISTRY_BRIDGE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_REGISTRY_BRIDGE_H

#include "umicom/ai_developer_experience/command_router.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperCommandRegistryBridge
    UmiAiDeveloperCommandRegistryBridge;

UmiStatus umi_ai_developer_command_registry_bridge_create(
    UmiCommandRegistry *registry,
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperCommandRegistryBridge **out_bridge);

void umi_ai_developer_command_registry_bridge_destroy(
    UmiAiDeveloperCommandRegistryBridge *bridge);

UmiStatus umi_ai_developer_command_registry_bridge_set_context(
    UmiAiDeveloperCommandRegistryBridge *bridge,
    const UmiAiDeveloperCommandContext *context);

UmiStatus umi_ai_developer_command_registry_bridge_register(
    UmiAiDeveloperCommandRegistryBridge *bridge);

#ifdef __cplusplus
}
#endif
#endif
