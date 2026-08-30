/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/bindings.c
 *
 * PURPOSE:
 *   Implement non-owning Studio composition bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/bindings.h"

#include <string.h>

void umi_studio_runtime_bindings_init(
    UmiStudioRuntimeBindings *bindings)
{
    if (bindings == NULL) return;
    (void)memset(bindings, 0, sizeof(*bindings));
    bindings->revision = 1U;
}

UmiStatus umi_studio_runtime_bind_shell(
    UmiStudioRuntimeBindings *bindings,
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellState *state,
    UmiApplicationShellLayout *layout)
{
    if (bindings == NULL || registry == NULL ||
        state == NULL || layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings->shell_registry = registry;
    bindings->shell_state = state;
    bindings->shell_layout = layout;
    bindings->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_bind_commands(
    UmiStudioRuntimeBindings *bindings,
    UmiCommandRegistry *commands)
{
    if (bindings == NULL || commands == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings->commands = commands;
    bindings->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_bind_ide(
    UmiStudioRuntimeBindings *bindings,
    UmiIdeIntegrationPlatform *ide)
{
    if (bindings == NULL || ide == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings->ide = ide;
    bindings->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_bind_documents(
    UmiStudioRuntimeBindings *bindings,
    UmiDocumentCoordinator *documents)
{
    if (bindings == NULL || documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings->documents = documents;
    bindings->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_bindings_validate(
    const UmiStudioRuntimeBindings *bindings)
{
    if (bindings == NULL ||
        bindings->shell_registry == NULL ||
        bindings->shell_state == NULL ||
        bindings->shell_layout == NULL ||
        bindings->commands == NULL ||
        bindings->ide == NULL ||
        bindings->documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
