/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/completion_provider.c
 *
 * PURPOSE:
 *   Implement validated completion requests and provider registration,
 *   capability selection, invocation, resolution and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/completion_provider.h"

#include <stdlib.h>
#include <string.h>

typedef struct CompletionProviderEntry {
    UmiEditorCompletionProviderDescriptor descriptor;
    UmiEditorCompletionProviderCallbacks callbacks;
    void *user_data;
} CompletionProviderEntry;

struct UmiEditorCompletionProviderRegistry {
    CompletionProviderEntry *entries;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Provide the validate descriptor operation used by this module and its client
 * applications.
 */
static UmiStatus validate_descriptor(
    const UmiEditorCompletionProviderDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION ||
        !terminated(descriptor->id, sizeof(descriptor->id)) ||
        descriptor->id[0] == '\0' ||
        !terminated(descriptor->label, sizeof(descriptor->label)) ||
        descriptor->label[0] == '\0' ||
        !terminated(descriptor->language_id,
                    sizeof(descriptor->language_id)) ||
        descriptor->language_id[0] == '\0' ||
        !terminated(descriptor->trigger_characters,
                    sizeof(descriptor->trigger_characters)) ||
        descriptor->source < UMI_EDITOR_COMPLETION_SOURCE_OTHER ||
        descriptor->source > UMI_EDITOR_COMPLETION_SOURCE_AI) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the validate callbacks operation used by this module and its client
 * applications.
 */
static UmiStatus validate_callbacks(
    const UmiEditorCompletionProviderCallbacks *callbacks)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (callbacks == NULL ||
        callbacks->struct_size != (uint32_t)sizeof(*callbacks) ||
        callbacks->api_version != UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION ||
        callbacks->provide == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve entries operation used by this module and its client applications. */
static UmiStatus reserve_entries(UmiEditorCompletionProviderRegistry *registry,
                                 size_t required)
{
    size_t capacity;
    CompletionProviderEntry *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U ? registry->capacity : 8U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (CompletionProviderEntry *)realloc(
        registry->entries, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->entries = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find entry operation used by this module and its client applications. */
static size_t find_entry(const UmiEditorCompletionProviderRegistry *registry,
                         const char *provider_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < registry->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->entries[position].descriptor.id, provider_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/*
 * Provide the editor completion request default operation used by this module and its
 * client applications.
 */
UmiEditorCompletionRequest umi_editor_completion_request_default(
    const char *document_id,
    const char *language_id,
    uint64_t request_id)
{
    UmiEditorCompletionRequest request;

    (void)memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    copy_text(request.document_id, sizeof(request.document_id), document_id);
    copy_text(request.language_id, sizeof(request.language_id), language_id);
    request.trigger_kind = UMI_EDITOR_COMPLETION_TRIGGER_INVOKED;
    request.maximum_candidates = 512U;
    request.request_id = request_id;
    request.allow_snippets = 1;
    request.allow_deprecated = 0;
    request.allow_ai = 1;
    request.allow_remote = 0;
    request.trusted_workspace = 0;
    return request;
}

/*
 * Check that editor completion request satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_editor_completion_request_validate(
    const UmiEditorCompletionRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION ||
        !terminated(request->document_id, sizeof(request->document_id)) ||
        request->document_id[0] == '\0' ||
        !terminated(request->document_uri, sizeof(request->document_uri)) ||
        !terminated(request->language_id, sizeof(request->language_id)) ||
        request->language_id[0] == '\0' ||
        !terminated(request->prefix, sizeof(request->prefix)) ||
        !terminated(request->line_prefix, sizeof(request->line_prefix)) ||
        !terminated(request->line_suffix, sizeof(request->line_suffix)) ||
        !terminated(request->trigger_character,
                    sizeof(request->trigger_character)) ||
        request->trigger_kind < UMI_EDITOR_COMPLETION_TRIGGER_INVOKED ||
        request->trigger_kind > UMI_EDITOR_COMPLETION_TRIGGER_AUTOMATIC ||
        request->maximum_candidates == 0U || request->request_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->trigger_kind == UMI_EDITOR_COMPLETION_TRIGGER_CHARACTER &&
        request->trigger_character[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor completion request is cancelled operation used by this module and its
 * client applications.
 */
int umi_editor_completion_request_is_cancelled(
    const UmiEditorCompletionRequest *request)
{
    return request != NULL && request->cancellation_probe != NULL &&
           request->cancellation_probe(request->cancellation_user_data) != 0;
}

/*
 * Initialise editor completion provider registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_completion_provider_registry_create(
    UmiEditorCompletionProviderRegistry **out_registry)
{
    UmiEditorCompletionProviderRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorCompletionProviderRegistry *)calloc(
        1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor completion provider registry so the same storage
 * can be reused safely.
 */
void umi_editor_completion_provider_registry_destroy(
    UmiEditorCompletionProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    free(registry->entries);
    registry->entries = NULL;
    free(registry);
}

/*
 * Release or reset state held by editor completion provider registry so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_completion_provider_registry_clear(
    UmiEditorCompletionProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry->count = 0U;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Add editor completion provider registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_editor_completion_provider_registry_register(
    UmiEditorCompletionProviderRegistry *registry,
    const UmiEditorCompletionProviderDescriptor *descriptor,
    const UmiEditorCompletionProviderCallbacks *callbacks,
    void *provider_user_data)
{
    CompletionProviderEntry entry;
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL ||
        validate_descriptor(descriptor) != UMI_STATUS_OK ||
        validate_callbacks(callbacks) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_entry(registry, descriptor->id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        status = reserve_entries(registry, registry->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = registry->count++;
    }
    (void)memset(&entry, 0, sizeof(entry));
    entry.descriptor = *descriptor;
    entry.descriptor.supports_manual_invocation =
        descriptor->supports_manual_invocation != 0;
    entry.descriptor.supports_trigger_characters =
        descriptor->supports_trigger_characters != 0;
    entry.descriptor.supports_automatic_invocation =
        descriptor->supports_automatic_invocation != 0;
    entry.descriptor.supports_incomplete_refresh =
        descriptor->supports_incomplete_refresh != 0;
    entry.descriptor.supports_resolution =
        descriptor->supports_resolution != 0;
    entry.descriptor.supports_commit_characters =
        descriptor->supports_commit_characters != 0;
    entry.descriptor.supports_snippets = descriptor->supports_snippets != 0;
    entry.descriptor.requires_network = descriptor->requires_network != 0;
    entry.descriptor.requires_workspace_trust =
        descriptor->requires_workspace_trust != 0;
    entry.descriptor.enabled = descriptor->enabled != 0;
    entry.callbacks = *callbacks;
    entry.user_data = provider_user_data;
    registry->entries[position] = entry;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove editor completion provider registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_editor_completion_provider_registry_unregister(
    UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < registry->count) {
        (void)memmove(&registry->entries[position],
                      &registry->entries[position + 1U],
                      (registry->count - position - 1U) *
                          sizeof(*registry->entries));
    }
    --registry->count;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor completion provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_completion_provider_registry_find(
    const UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    UmiEditorCompletionProviderDescriptor *out_descriptor)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->entries[position].descriptor;
    return UMI_STATUS_OK;
}

/*
 * Find editor completion provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_completion_provider_registry_at(
    const UmiEditorCompletionProviderRegistry *registry,
    size_t position,
    UmiEditorCompletionProviderDescriptor *out_descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->entries[position].descriptor;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor completion provider supports request operation used by this module
 * and its client applications.
 */
int umi_editor_completion_provider_supports_request(
    const UmiEditorCompletionProviderDescriptor *descriptor,
    const UmiEditorCompletionRequest *request)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (validate_descriptor(descriptor) != UMI_STATUS_OK ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK ||
        !descriptor->enabled ||
        (strcmp(descriptor->language_id, "*") != 0 &&
         strcmp(descriptor->language_id, request->language_id) != 0)) {
        return 0;
    }
    /* Select the behaviour associated with the requested command or state value. */
    switch (request->trigger_kind) {
        case UMI_EDITOR_COMPLETION_TRIGGER_INVOKED:
            return descriptor->supports_manual_invocation;
        case UMI_EDITOR_COMPLETION_TRIGGER_CHARACTER:
            return descriptor->supports_trigger_characters &&
                   request->trigger_character[0] != '\0' &&
                   strchr(descriptor->trigger_characters,
                          request->trigger_character[0]) != NULL;
        case UMI_EDITOR_COMPLETION_TRIGGER_INCOMPLETE_REFRESH:
            return descriptor->supports_incomplete_refresh;
        case UMI_EDITOR_COMPLETION_TRIGGER_AUTOMATIC:
            return descriptor->supports_automatic_invocation;
        default:
            return 0;
    }
}

/*
 * Provide the editor completion provider registry invoke operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_completion_provider_registry_invoke(
    const UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    const UmiEditorCompletionRequest *request,
    UmiEditorCompletionCandidateSink sink,
    void *sink_user_data,
    UmiEditorCompletionProviderResponse *out_response)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || sink == NULL ||
        out_response == NULL ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_editor_completion_provider_supports_request(
            &registry->entries[position].descriptor, request)) {
        return UMI_STATUS_UNAVAILABLE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_completion_request_is_cancelled(request)) {
        return UMI_STATUS_CANCELLED;
    }
    (void)memset(out_response, 0, sizeof(*out_response));
    out_response->struct_size = (uint32_t)sizeof(*out_response);
    out_response->api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    return registry->entries[position].callbacks.provide(
        request,
        sink,
        sink_user_data,
        out_response,
        registry->entries[position].user_data);
}

/*
 * Provide the editor completion provider registry resolve operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_completion_provider_registry_resolve(
    const UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    const UmiEditorCompletionRequest *request,
    UmiEditorCompletionCandidate *in_out_candidate)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || in_out_candidate == NULL ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK ||
        umi_editor_completion_candidate_validate(in_out_candidate) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!registry->entries[position].descriptor.supports_resolution ||
        registry->entries[position].callbacks.resolve == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_completion_request_is_cancelled(request)) {
        return UMI_STATUS_CANCELLED;
    }
    return registry->entries[position].callbacks.resolve(
        request,
        in_out_candidate,
        registry->entries[position].user_data);
}

/*
 * Provide the editor completion provider registry cancel operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_completion_provider_registry_cancel(
    const UmiEditorCompletionProviderRegistry *registry,
    const char *provider_id,
    uint64_t request_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        request_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_entry(registry, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry->entries[position].callbacks.cancel == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return registry->entries[position].callbacks.cancel(
        request_id, registry->entries[position].user_data);
}

/*
 * Provide the editor completion provider registry snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_completion_provider_registry_snapshot(
    const UmiEditorCompletionProviderRegistry *registry,
    UmiEditorCompletionProviderRegistrySnapshot *out_snapshot)
{
    size_t position;
    size_t comparison;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_COMPLETION_PROVIDER_API_VERSION;
    out_snapshot->provider_count = registry->count;
    out_snapshot->revision = registry->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < registry->count; ++position) {
        const UmiEditorCompletionProviderDescriptor *descriptor =
            &registry->entries[position].descriptor;
        int first_language = 1;
        /* Apply this operation only while the related capability or state is available. */
        if (descriptor->enabled) ++out_snapshot->enabled_provider_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
            ++out_snapshot->ai_provider_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->requires_network) {
            ++out_snapshot->remote_provider_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->supports_resolution) {
            ++out_snapshot->resolving_provider_count;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < position; ++comparison) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(descriptor->language_id,
                       registry->entries[comparison].descriptor.language_id) ==
                0) {
                first_language = 0;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (first_language) ++out_snapshot->language_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor completion provider registry without
 * changing their state.
 */
size_t umi_editor_completion_provider_registry_count(
    const UmiEditorCompletionProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the editor completion provider registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_completion_provider_registry_revision(
    const UmiEditorCompletionProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
