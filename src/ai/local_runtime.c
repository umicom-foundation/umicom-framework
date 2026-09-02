/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/local_runtime.c
 *
 * PURPOSE:
 *   Configure a local model-runtime endpoint or executable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Local inference remains replaceable: llama.cpp, a native Umicom engine or another local server can implement the provider contract.
 */

#include "umicom/ai/local_runtime.h"
#include <stddef.h>

/* Check that ai local runtime satisfies its contract before another service relies on it. */
UmiStatus umi_ai_local_runtime_validate(const UmiAiLocalRuntimeConfig *config)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || config->executable[0] == '\0' || config->model_path[0] == '\0' || config->port == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
