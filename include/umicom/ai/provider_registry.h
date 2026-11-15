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

/*
 * The registry gives Studio one provider catalogue instead of letting every pane keep private provider pointers.
 */

#ifndef INCLUDE_UMICOM_AI_PROVIDER_REGISTRY_H
#define INCLUDE_UMICOM_AI_PROVIDER_REGISTRY_H

#include <stddef.h>
#include "umicom/ai/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai provider registry data shared with callers of this public contract.
 */
typedef struct UmiAiProviderRegistry {
    UmiAiProvider providers[UMI_AI_MAX_PROVIDERS];
    size_t count;
} UmiAiProviderRegistry;

/**
 * Initialise ai provider registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_provider_registry_init(UmiAiProviderRegistry *registry);
/**
 * Check the bounded registry before looking up or invoking any provider.
 * Stored identifiers remain borrowed, readable, NUL-terminated and stable
 * until release. Structural validation cannot make dangling pointers safe.
 * Callers must serialize access; validation does not acquire a lifetime lease.
 */
UmiStatus UmiAiProviderRegistryValidate(const UmiAiProviderRegistry *registry);
/**
 * Release owned instances in reverse registration order. Invalid registry
 * state returns INVALID_STATE without invoking callbacks or changing storage.
 * During callbacks registry APIs refuse reentry. Callbacks must not directly
 * mutate or reinitialize this registry, and must return normally.
 */
UmiStatus UmiAiProviderRegistryRelease(UmiAiProviderRegistry *registry);
/* Destroy callbacks run once for each successfully registered owned instance.
 * This compatibility wrapper leaves invalid state intact; use Release to
 * observe the reason. Initialization is only for fresh or released storage. */
void umi_ai_provider_registry_destroy(UmiAiProviderRegistry *registry);
/**
 * Add ai provider registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ai_provider_registry_add(UmiAiProviderRegistry *registry,
                                       const UmiAiProvider *provider);
/**
 * Find ai provider registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiAiProvider *umi_ai_provider_registry_find(UmiAiProviderRegistry *registry,
                                               const char *provider_id);

/** Read-only synchronous generation/health activity check. BUSY means this registry
 * appears in the current thread callback stack. This does not validate table
 * contents, acquire a lease, or synchronize threads. NULL is INVALID_ARGUMENT.
 * Registry add/release return BUSY during active callbacks; void initialize
 * and destroy retain the owner. Reads remain available. Callbacks must return
 * normally and must not directly mutate, copy or free their live owner. */
UmiStatus UmiAiProviderRegistryCheckIdle(const UmiAiProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
