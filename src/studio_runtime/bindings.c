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

/*
 * Initialise studio runtime bindings from caller-provided values so later operations
 * receive a known state.
 */
void umi_studio_runtime_bindings_init(
    UmiStudioRuntimeBindings *bindings)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return;
    (void)memset(bindings, 0, sizeof(*bindings));
    bindings->revision = 1U;
}

/*
 * Provide the studio runtime bind shell operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_runtime_bind_shell(
    UmiStudioRuntimeBindings *bindings,
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellState *state,
    UmiApplicationShellLayout *layout)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the studio runtime bind commands operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_runtime_bind_commands(
    UmiStudioRuntimeBindings *bindings,
    UmiCommandRegistry *commands)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || commands == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings->commands = commands;
    bindings->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the studio runtime bind ide operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_runtime_bind_ide(
    UmiStudioRuntimeBindings *bindings,
    UmiIdeIntegrationPlatform *ide)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || ide == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings->ide = ide;
    bindings->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the studio runtime bind documents operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_runtime_bind_documents(
    UmiStudioRuntimeBindings *bindings,
    UmiDocumentCoordinator *documents)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bindings->documents = documents;
    bindings->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Check that studio runtime bindings satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_studio_runtime_bindings_validate(
    const UmiStudioRuntimeBindings *bindings)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
