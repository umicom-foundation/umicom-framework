/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/command_registry_bridge.h
 *
 * PURPOSE:
 *   Register AI Developer Experience commands in the Framework's authoritative
 *   runtime command registry so menus, toolbars, palettes, automation and AI can
 *   discover the same command surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_REGISTRY_BRIDGE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_REGISTRY_BRIDGE_H

#include "umicom/ai_developer_experience/command_router.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai developer command registry bridge data shared with callers of this
 * public contract.
 */
typedef struct UmiAiDeveloperCommandRegistryBridge
    UmiAiDeveloperCommandRegistryBridge;

/**
 * Initialise ai developer command registry bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ai_developer_command_registry_bridge_create(
    UmiCommandRegistry *registry,
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperCommandRegistryBridge **out_bridge);

/**
 * Release or reset state held by ai developer command registry bridge so the same storage
 * can be reused safely.
 */
void umi_ai_developer_command_registry_bridge_destroy(
    UmiAiDeveloperCommandRegistryBridge *bridge);

/**
 * Provide the ai developer command registry bridge set context operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_command_registry_bridge_set_context(
    UmiAiDeveloperCommandRegistryBridge *bridge,
    const UmiAiDeveloperCommandContext *context);

/**
 * Add ai developer command registry bridge only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_ai_developer_command_registry_bridge_register(
    UmiAiDeveloperCommandRegistryBridge *bridge);

#ifdef __cplusplus
}
#endif
#endif
