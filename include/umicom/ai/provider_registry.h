/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/provider_registry.h
 *
 * PURPOSE:
 *   Register and resolve a bounded set of AI providers by stable identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The registry gives Studio one provider catalogue instead of letting every pane keep private provider pointers.
 */

#ifndef INCLUDE_UMICOM_AI_PROVIDER_REGISTRY_H
#define INCLUDE_UMICOM_AI_PROVIDER_REGISTRY_H

#include <stddef.h>
#include "umicom/ai/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiProviderRegistry {
    UmiAiProvider providers[UMI_AI_MAX_PROVIDERS];
    size_t count;
} UmiAiProviderRegistry;

void umi_ai_provider_registry_init(UmiAiProviderRegistry *registry);
UmiStatus umi_ai_provider_registry_add(UmiAiProviderRegistry *registry,
                                       const UmiAiProvider *provider);
UmiAiProvider *umi_ai_provider_registry_find(UmiAiProviderRegistry *registry,
                                               const char *provider_id);

#ifdef __cplusplus
}
#endif

#endif
