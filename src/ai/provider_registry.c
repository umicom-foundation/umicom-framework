/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/provider_registry.c
 *
 * PURPOSE:
 *   Register and resolve AI providers by identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Registration copies the small function table, while each provider keeps its private implementation behind the opaque instance pointer.
 */

#include "umicom/ai/provider_registry.h"
#include "provider_registry_internal.h"
#include <stddef.h>
#include <stdint.h>

#include <string.h>

/* Keep public registry layouts unchanged. Each synchronous call links a stack
 * frame while the provider owns borrowed inputs. Walking every frame also
 * protects an outer registry when a different registry is invoked recursively.
 * Thread-local state detects reentry only; hosts still serialize shared owners. */
typedef struct AiProviderInvocationFrame {
    const UmiAiProviderRegistry *registry;
    struct AiProviderInvocationFrame *previous;
} AiProviderInvocationFrame;
static _Thread_local AiProviderInvocationFrame *aiProviderInvocations;

UmiStatus UmiAiProviderRegistryCheckIdle(const UmiAiProviderRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (const AiProviderInvocationFrame *frame = aiProviderInvocations;
         frame != NULL; frame = frame->previous)
        if (frame->registry == registry) return UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}

/* Generation and health refresh share one scoped callback runner so all exit statuses release the same ABI-preserving ownership frame. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiProviderRegistryInvoke(UmiAiProviderRegistry *registry,
    const UmiAiProvider *selected, const UmiAiRequest *request,
    UmiAiResponse *response, int *dispatched)
{
    if (dispatched == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *dispatched = 0;
    if (registry == NULL || selected == NULL || request == NULL || response == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiProviderRegistryCheckIdle(registry);
    if (status != UMI_STATUS_OK) return status;
    /* Admission already validated the selected table; snapshot it so the
     * callback does not depend on its slot while returning through Framework. */
    const UmiAiProvider provider = *selected;
    AiProviderInvocationFrame frame = {registry, aiProviderInvocations};
    aiProviderInvocations = &frame;
    *dispatched = 1;
    status = provider.generate(provider.instance, request, response);
    aiProviderInvocations = frame.previous;
    return status;
}

#endif
UmiStatus UmiAiProviderRegistryRunOwned(UmiAiProviderRegistry *registry,
    UmiAiProviderOwnedCallback callback, void *context, int *dispatched)
{
    if (dispatched == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *dispatched = 0;
    if (registry == NULL || callback == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiProviderRegistryCheckIdle(registry);
    if (status != UMI_STATUS_OK) return status;
    AiProviderInvocationFrame frame = {registry, aiProviderInvocations};
    aiProviderInvocations = &frame;
    *dispatched = 1;
    status = callback(context);
    aiProviderInvocations = frame.previous;
    return status;
}

typedef struct AiGenerationCall {
    UmiAiProvider provider;
    const UmiAiRequest *request;
    UmiAiResponse *response;
} AiGenerationCall;

static UmiStatus InvokeGenerationCallback(void *context)
{
    const AiGenerationCall *call = context;
    return call->provider.generate(call->provider.instance, call->request, call->response);
}

UmiStatus UmiAiProviderRegistryInvoke(UmiAiProviderRegistry *registry,
    const UmiAiProvider *selected, const UmiAiRequest *request,
    UmiAiResponse *response, int *dispatched)
{
    if (dispatched == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *dispatched = 0;
    if (registry == NULL || selected == NULL || request == NULL || response == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Admission owns selection; the common runner owns callback lifetime. */
    AiGenerationCall call = {*selected, request, response};
    return UmiAiProviderRegistryRunOwned(registry, InvokeGenerationCallback, &call, dispatched);
}

/*
 * Initialise ai provider registry from caller-provided values so later operations receive
 * a known state.
 */
/* Initialization refuses an active callback owner so even the compatibility void API cannot erase provider lifetime state. The former implementation is retained for owner review. */
#if 0
void umi_ai_provider_registry_init(UmiAiProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

#endif
void umi_ai_provider_registry_init(UmiAiProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        if (UmiAiProviderRegistryCheckIdle(registry) != UMI_STATUS_OK) return;
        (void)memset(registry, 0, sizeof(*registry));
    }
}

/*
 * Release or reset state held by ai provider registry so the same storage can be reused
 * safely.
 */
/* Checked release replaces unchecked count traversal. The previous implementation is retained for ownership review; valid registries retain reverse-order cleanup and the legacy entry point. The former implementation is retained for owner review. */
#if 0
void umi_ai_provider_registry_destroy(UmiAiProviderRegistry *registry)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    /* Reverse registration order mirrors dependency-friendly stack cleanup. */
    for (index = registry->count; index > 0U; --index) {
        UmiAiProvider *provider = &registry->providers[index - 1U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (provider->destroy != NULL && provider->instance != NULL) {
            provider->destroy(provider->instance);
            provider->instance = NULL;
        }
    }
    (void)memset(registry, 0, sizeof(*registry));
}

#endif
UmiStatus UmiAiProviderRegistryValidate(const UmiAiProviderRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->count > UMI_AI_MAX_PROVIDERS) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < registry->count; ++index) {
        const UmiAiProvider *provider = &registry->providers[index];
        if (umi_ai_provider_validate(provider) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t earlier = 0U; earlier < index; ++earlier) {
            if (strcmp(registry->providers[earlier].provider_id, provider->provider_id) == 0)
                return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}

/* Release must retain active provider instances until synchronous generation returns. Destructor sentinel behavior and reverse release order remain unchanged. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiProviderRegistryRelease(UmiAiProviderRegistry *registry)
{
    UmiAiProvider owned[UMI_AI_MAX_PROVIDERS];
    UmiStatus status = UmiAiProviderRegistryValidate(registry);
    if (status != UMI_STATUS_OK) return status;
    const size_t count = registry->count;
    memcpy(owned, registry->providers, count * sizeof(owned[0]));
    /* A callback may call the API again. Mark teardown before invoking one,
     * and use the snapshot so no callback can release an entry twice through
     * supported registry operations. No allocation is needed during cleanup. */
    registry->count = SIZE_MAX;
    for (size_t index = count; index > 0U; --index) {
        UmiAiProvider *provider = &owned[index - 1U];
        if (provider->destroy != NULL && provider->instance != NULL)
            provider->destroy(provider->instance);
    }
    memset(registry, 0, sizeof(*registry));
    return UMI_STATUS_OK;
}

#endif
UmiStatus UmiAiProviderRegistryRelease(UmiAiProviderRegistry *registry)
{
    UmiAiProvider owned[UMI_AI_MAX_PROVIDERS];
    UmiStatus status = UmiAiProviderRegistryCheckIdle(registry);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiProviderRegistryValidate(registry);
    if (status != UMI_STATUS_OK) return status;
    const size_t count = registry->count;
    memcpy(owned, registry->providers, count * sizeof(owned[0]));
    /* A callback may call the API again. Mark teardown before invoking one,
     * and use the snapshot so no callback can release an entry twice through
     * supported registry operations. No allocation is needed during cleanup. */
    registry->count = SIZE_MAX;
    for (size_t index = count; index > 0U; --index) {
        UmiAiProvider *provider = &owned[index - 1U];
        if (provider->destroy != NULL && provider->instance != NULL)
            provider->destroy(provider->instance);
    }
    memset(registry, 0, sizeof(*registry));
    return UMI_STATUS_OK;
}

void umi_ai_provider_registry_destroy(UmiAiProviderRegistry *registry)
{
    if (registry != NULL) (void)UmiAiProviderRegistryRelease(registry);
}

/* Add ai provider registry only after its inputs and available capacity have been checked. */
/* Registration now validates existing storage before reading entries. Ownership transfers only after successful insertion; original duplicate and capacity results are retained below for review. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_provider_registry_add(UmiAiProviderRegistry *registry,
                                       const UmiAiProvider *provider)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || umi_ai_provider_validate(provider) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ai_provider_registry_find(registry, provider->provider_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_AI_MAX_PROVIDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->providers[registry->count++] = *provider;
    return UMI_STATUS_OK;
}

#endif
/* Insertion is refused during invocation so callback ownership and registry membership remain stable through supported mutation APIs. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_provider_registry_add(UmiAiProviderRegistry *registry,
                                       const UmiAiProvider *provider)
{
    if (registry == NULL || umi_ai_provider_validate(provider) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiProviderRegistryValidate(registry);
    if (status != UMI_STATUS_OK) return status;
    /* Duplicate identity keeps its existing result even when the table is full. */
    for (size_t index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index].provider_id, provider->provider_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count == UMI_AI_MAX_PROVIDERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->providers[registry->count++] = *provider;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_provider_registry_add(UmiAiProviderRegistry *registry,
                                       const UmiAiProvider *provider)
{
    if (registry == NULL || umi_ai_provider_validate(provider) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiProviderRegistryCheckIdle(registry);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiProviderRegistryValidate(registry);
    if (status != UMI_STATUS_OK) return status;
    /* Duplicate identity keeps its existing result even when the table is full. */
    for (size_t index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index].provider_id, provider->provider_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count == UMI_AI_MAX_PROVIDERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->providers[registry->count++] = *provider;
    return UMI_STATUS_OK;
}

/*
 * Find ai provider registry while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Lookup now refuses malformed storage before traversing identifiers. The pointer-returning compatibility interface still reports an unavailable match as NULL; checked callers can obtain the structural error from Validate. The former implementation is retained for owner review. */
#if 0
UmiAiProvider *umi_ai_provider_registry_find(UmiAiProviderRegistry *registry,
                                               const char *provider_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->providers[index].provider_id, provider_id) == 0) {
            return &registry->providers[index];
        }
    }
    return NULL;
}

#endif
UmiAiProvider *umi_ai_provider_registry_find(UmiAiProviderRegistry *registry,
                                               const char *provider_id)
{
    if (provider_id == NULL || UmiAiProviderRegistryValidate(registry) != UMI_STATUS_OK)
        return NULL;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index].provider_id, provider_id) == 0)
            return &registry->providers[index];
    }
    return NULL;
}
