/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/local_runtime.c
 *
 * PURPOSE:
 *   Configure a local model-runtime endpoint or executable.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Local inference remains replaceable: llama.cpp, a native Umicom engine or another local server can implement the provider contract.
 */

#include "umicom/ai/local_runtime.h"
#include <stddef.h>

UmiStatus umi_ai_local_runtime_validate(const UmiAiLocalRuntimeConfig *config)
{
    if (config == NULL || config->executable[0] == '\0' || config->model_path[0] == '\0' || config->port == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
