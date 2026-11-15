/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/provider_registry_internal.h
 *
 * PURPOSE:
 *   Share guarded synchronous provider invocation with runtime dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_PROVIDER_REGISTRY_INTERNAL_H
#define UMICOM_AI_PROVIDER_REGISTRY_INTERNAL_H

#include "umicom/ai/provider_registry.h"

/* Runtime owns request and policy admission. This internal boundary owns the
 * synchronous callback lifetime. selected is a borrowed entry from registry.
 * dispatched is reset before admission and set only immediately before a call.
 * Inputs stay valid and all callbacks return normally on the same thread. */
UmiStatus UmiAiProviderRegistryInvoke(UmiAiProviderRegistry *registry,
    const UmiAiProvider *selected, const UmiAiRequest *request,
    UmiAiResponse *response, int *dispatched);

/* A complete serialized owner operation may contain several provider/probe
 * callbacks and publication steps. Keep its registry alive for the whole scope.
 * This private runner neither validates registry contents nor grants policy. */
typedef UmiStatus (*UmiAiProviderOwnedCallback)(void *context);
UmiStatus UmiAiProviderRegistryRunOwned(UmiAiProviderRegistry *registry,
    UmiAiProviderOwnedCallback callback, void *context, int *dispatched);

#endif
