/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/local_runtime.h
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

/* BEGINNER NOTE:
 * Local inference remains replaceable: llama.cpp, a native Umicom engine or another local server can implement the provider contract.
 */

#ifndef INCLUDE_UMICOM_AI_LOCAL_RUNTIME_H
#define INCLUDE_UMICOM_AI_LOCAL_RUNTIME_H

#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiLocalRuntimeConfig {
    char executable[UMI_AI_TEXT_CAPACITY];
    char model_path[UMI_AI_TEXT_CAPACITY];
    uint16_t port;
} UmiAiLocalRuntimeConfig;

UmiStatus umi_ai_local_runtime_validate(const UmiAiLocalRuntimeConfig *config);

#ifdef __cplusplus
}
#endif

#endif
