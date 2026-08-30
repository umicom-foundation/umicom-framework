/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/bindings.h
 *
 * PURPOSE:
 *   Bind the existing generic Application Shell, command registry and IDE
 *   integration platform into one non-owning Studio composition contract.
 *
 * OWNERSHIP:
 *   The binding owns none of the pointed-to services. Lifetimes remain with the
 *   Framework application composition root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_BINDINGS_H
#define UMICOM_STUDIO_RUNTIME_BINDINGS_H

#include "umicom/application_shell/layout.h"
#include "umicom/application_shell/registry.h"
#include "umicom/application_shell/state.h"
#include "umicom/document/coordinator.h"
#include "umicom/ide_integration/platform.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeBindings {
    UmiApplicationShellRegistry *shell_registry;
    UmiApplicationShellState *shell_state;
    UmiApplicationShellLayout *shell_layout;
    UmiCommandRegistry *commands;
    UmiIdeIntegrationPlatform *ide;
    UmiDocumentCoordinator *documents;
    uint64_t revision;
} UmiStudioRuntimeBindings;

void umi_studio_runtime_bindings_init(
    UmiStudioRuntimeBindings *bindings);

UmiStatus umi_studio_runtime_bind_shell(
    UmiStudioRuntimeBindings *bindings,
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellState *state,
    UmiApplicationShellLayout *layout);

UmiStatus umi_studio_runtime_bind_commands(
    UmiStudioRuntimeBindings *bindings,
    UmiCommandRegistry *commands);

UmiStatus umi_studio_runtime_bind_ide(
    UmiStudioRuntimeBindings *bindings,
    UmiIdeIntegrationPlatform *ide);

UmiStatus umi_studio_runtime_bind_documents(
    UmiStudioRuntimeBindings *bindings,
    UmiDocumentCoordinator *documents);

UmiStatus umi_studio_runtime_bindings_validate(
    const UmiStudioRuntimeBindings *bindings);

#ifdef __cplusplus
}
#endif
#endif
