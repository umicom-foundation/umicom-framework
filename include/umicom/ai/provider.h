/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/provider.h
 *
 * PURPOSE:
 *   Define the stable provider-neutral function table used by local, remote and AuthorEngine AI adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Providers expose a C function table, so implementations may be written in C, C++, Rust or another language without leaking private runtime types.
 */

#ifndef INCLUDE_UMICOM_AI_PROVIDER_H
#define INCLUDE_UMICOM_AI_PROVIDER_H

#include <stdint.h>
#include "umicom/ai/request.h"
#include "umicom/ai/response.h"
#include "umicom/ai/health.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiProvider {
    uint32_t structure_size;
    uint32_t abi_version;
    const char *provider_id;
    UmiAiProviderKind kind;
    void *instance;
    UmiStatus (*generate)(void *instance,
                          const UmiAiRequest *request,
                          UmiAiResponse *response);
    UmiStatus (*health)(void *instance, UmiAiHealth *health);
    void (*destroy)(void *instance);
} UmiAiProvider;

UmiStatus umi_ai_provider_validate(const UmiAiProvider *provider);

#ifdef __cplusplus
}
#endif

#endif
