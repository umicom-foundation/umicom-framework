/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/inline_suggestion.c
 *
 * PURPOSE:
 *   Implement inline suggestion collections and provider registration,
 *   capability selection, invocation, streaming metadata and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/inline_suggestion.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorInlineSuggestionCollection {
    UmiEditorInlineSuggestion *suggestions;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

typedef struct InlineProviderEntry {
    UmiEditorInlineSuggestionProviderDescriptor descriptor;
    UmiEditorInlineSuggestionProviderCallbacks callbacks;
    void *user_data;
} InlineProviderEntry;

struct UmiEditorInlineSuggestionProviderRegistry {
    InlineProviderEntry *entries;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus reserve_suggestions(
    UmiEditorInlineSuggestionCollection *collection,
    size_t required)
{
    size_t capacity;
    UmiEditorInlineSuggestion *replacement;

    if (required <= collection->capacity) return UMI_STATUS_OK;
    capacity = collection->capacity > 0U ? collection->capacity : 8U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorInlineSuggestion *)realloc(
        collection->suggestions, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    collection->suggestions = replacement;
    collection->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_suggestion(
    const UmiEditorInlineSuggestionCollection *collection,
    const char *provider_id,
    const char *suggestion_id)
{
    size_t position;

    if (collection == NULL || provider_id == NULL || suggestion_id == NULL) {
        return SIZE_MAX;
    }
    for (position = 0U; position < collection->count; ++position) {
        if (strcmp(collection->suggestions[position].provider_id,
                   provider_id) == 0 &&
            strcmp(collection->suggestions[position].id, suggestion_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_inline_suggestion_validate(
    const UmiEditorInlineSuggestion *suggestion)
{
    if (suggestion == NULL ||
        suggestion->struct_size != (uint32_t)sizeof(*suggestion) ||
        suggestion->api_version != UMI_EDITOR_INLINE_SUGGESTION_API_VERSION ||
        !terminated(suggestion->id, sizeof(suggestion->id)) ||
        suggestion->id[0] == '\0' ||
        !terminated(suggestion->provider_id,
                    sizeof(suggestion->provider_id)) ||
        suggestion->provider_id[0] == '\0' ||
        !terminated(suggestion->document_id,
                    sizeof(suggestion->document_id)) ||
        !terminated(suggestion->label, sizeof(suggestion->label)) ||
        !terminated(suggestion->text, sizeof(suggestion->text)) ||
        suggestion->text[0] == '\0' ||
        !terminated(suggestion->prefix_context,
                    sizeof(suggestion->prefix_context)) ||
        !terminated(suggestion->suffix_context,
                    sizeof(suggestion->suffix_context)) ||
        suggestion->replace_end_offset < suggestion->replace_start_offset ||
        suggestion->confidence_milli < 0 ||
        suggestion->confidence_milli > 1000 ||
        suggestion->source < UMI_EDITOR_COMPLETION_SOURCE_OTHER ||
        suggestion->source > UMI_EDITOR_COMPLETION_SOURCE_AI ||
        (suggestion->flags &
         ~(UmiEditorInlineSuggestionFlags)
             UMI_EDITOR_INLINE_SUGGESTION_ALL_FLAGS) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiEditorInlineSuggestion normalise_suggestion(
    const UmiEditorInlineSuggestion *suggestion)
{
    UmiEditorInlineSuggestion stored = *suggestion;

    if (stored.source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
        stored.flags |= UMI_EDITOR_INLINE_SUGGESTION_AI;
    }
    return stored;
}

UmiStatus umi_editor_inline_suggestion_collection_create(
    UmiEditorInlineSuggestionCollection **out_collection)
{
    UmiEditorInlineSuggestionCollection *collection;

    if (out_collection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_collection = NULL;
    collection = (UmiEditorInlineSuggestionCollection *)calloc(
        1U, sizeof(*collection));
    if (collection == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    collection->revision = 1U;
    *out_collection = collection;
    return UMI_STATUS_OK;
}

void umi_editor_inline_suggestion_collection_destroy(
    UmiEditorInlineSuggestionCollection *collection)
{
    if (collection == NULL) return;
    free(collection->suggestions);
    collection->suggestions = NULL;
    free(collection);
}

UmiStatus umi_editor_inline_suggestion_collection_clear(
    UmiEditorInlineSuggestionCollection *collection)
{
    if (collection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    collection->count = 0U;
    collection->revision = next_revision(collection->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_collection_upsert(
    UmiEditorInlineSuggestionCollection *collection,
    const UmiEditorInlineSuggestion *suggestion)
{
    size_t position;
    UmiStatus status;

    if (collection == NULL ||
        umi_editor_inline_suggestion_validate(suggestion) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_suggestion(collection,
                               suggestion->provider_id,
                               suggestion->id);
    if (position == SIZE_MAX) {
        status = reserve_suggestions(collection, collection->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = collection->count++;
    }
    collection->suggestions[position] = normalise_suggestion(suggestion);
    collection->revision = next_revision(collection->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_collection_find(
    const UmiEditorInlineSuggestionCollection *collection,
    const char *provider_id,
    const char *suggestion_id,
    UmiEditorInlineSuggestion *out_suggestion)
{
    size_t position;

    if (collection == NULL || provider_id == NULL || suggestion_id == NULL ||
        out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_suggestion(collection, provider_id, suggestion_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_suggestion = collection->suggestions[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_collection_at(
    const UmiEditorInlineSuggestionCollection *collection,
    size_t position,
    UmiEditorInlineSuggestion *out_suggestion)
{
    if (collection == NULL || out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= collection->count) return UMI_STATUS_NOT_FOUND;
    *out_suggestion = collection->suggestions[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_collection_snapshot(
    const UmiEditorInlineSuggestionCollection *collection,
    UmiEditorInlineSuggestionCollectionSnapshot *out_snapshot)
{
    size_t position;
    size_t comparison;

    if (collection == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    out_snapshot->suggestion_count = collection->count;
    out_snapshot->revision = collection->revision;
    for (position = 0U; position < collection->count; ++position) {
        const UmiEditorInlineSuggestion *suggestion =
            &collection->suggestions[position];
        int first_provider = 1;
        for (comparison = 0U; comparison < position; ++comparison) {
            if (strcmp(suggestion->provider_id,
                       collection->suggestions[comparison].provider_id) == 0) {
                first_provider = 0;
                break;
            }
        }
        if (first_provider) ++out_snapshot->provider_count;
        if ((suggestion->flags & UMI_EDITOR_INLINE_SUGGESTION_AI) != 0U) {
            ++out_snapshot->ai_count;
        }
        if ((suggestion->flags & UMI_EDITOR_INLINE_SUGGESTION_REMOTE) != 0U) {
            ++out_snapshot->remote_count;
        }
        if ((suggestion->flags &
             UMI_EDITOR_INLINE_SUGGESTION_STREAMING) != 0U) {
            ++out_snapshot->streaming_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_inline_suggestion_collection_count(
    const UmiEditorInlineSuggestionCollection *collection)
{
    return collection != NULL ? collection->count : 0U;
}

uint64_t umi_editor_inline_suggestion_collection_revision(
    const UmiEditorInlineSuggestionCollection *collection)
{
    return collection != NULL ? collection->revision : 0U;
}

int umi_editor_inline_suggestion_accepts_typed_prefix(
    const UmiEditorInlineSuggestion *suggestion,
    const char *typed_text)
{
    const size_t typed_length = typed_text != NULL ? strlen(typed_text) : 0U;

    if (umi_editor_inline_suggestion_validate(suggestion) != UMI_STATUS_OK ||
        typed_text == NULL || typed_length == 0U) {
        return 0;
    }
    return strncmp(suggestion->text, typed_text, typed_length) == 0;
}

static UmiStatus validate_provider_descriptor(
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor)
{
    if (descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_EDITOR_INLINE_SUGGESTION_API_VERSION ||
        !terminated(descriptor->id, sizeof(descriptor->id)) ||
        descriptor->id[0] == '\0' ||
        !terminated(descriptor->label, sizeof(descriptor->label)) ||
        descriptor->label[0] == '\0' ||
        !terminated(descriptor->language_id,
                    sizeof(descriptor->language_id)) ||
        descriptor->language_id[0] == '\0' ||
        descriptor->source < UMI_EDITOR_COMPLETION_SOURCE_OTHER ||
        descriptor->source > UMI_EDITOR_COMPLETION_SOURCE_AI) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus validate_provider_callbacks(
    const UmiEditorInlineSuggestionProviderCallbacks *callbacks)
{
    if (callbacks == NULL ||
        callbacks->struct_size != (uint32_t)sizeof(*callbacks) ||
        callbacks->api_version != UMI_EDITOR_INLINE_SUGGESTION_API_VERSION ||
        callbacks->provide == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_provider_entries(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    size_t required)
{
    size_t capacity;
    InlineProviderEntry *replacement;

    if (required <= registry->capacity) return UMI_STATUS_OK;
    capacity = registry->capacity > 0U ? registry->capacity : 8U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (InlineProviderEntry *)realloc(
        registry->entries, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->entries = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_provider_entry(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id)
{
    size_t position;

    if (registry == NULL || provider_id == NULL) return SIZE_MAX;
    for (position = 0U; position < registry->count; ++position) {
        if (strcmp(registry->entries[position].descriptor.id, provider_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_inline_suggestion_provider_registry_create(
    UmiEditorInlineSuggestionProviderRegistry **out_registry)
{
    UmiEditorInlineSuggestionProviderRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorInlineSuggestionProviderRegistry *)calloc(
        1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_editor_inline_suggestion_provider_registry_destroy(
    UmiEditorInlineSuggestionProviderRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->entries);
    registry->entries = NULL;
    free(registry);
}

UmiStatus umi_editor_inline_suggestion_provider_registry_register(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor,
    const UmiEditorInlineSuggestionProviderCallbacks *callbacks,
    void *provider_user_data)
{
    InlineProviderEntry entry;
    size_t position;
    UmiStatus status;

    if (registry == NULL ||
        validate_provider_descriptor(descriptor) != UMI_STATUS_OK ||
        validate_provider_callbacks(callbacks) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, descriptor->id);
    if (position == SIZE_MAX) {
        status = reserve_provider_entries(registry, registry->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        position = registry->count++;
    }
    (void)memset(&entry, 0, sizeof(entry));
    entry.descriptor = *descriptor;
    entry.descriptor.supports_manual_invocation =
        descriptor->supports_manual_invocation != 0;
    entry.descriptor.supports_automatic_invocation =
        descriptor->supports_automatic_invocation != 0;
    entry.descriptor.supports_streaming = descriptor->supports_streaming != 0;
    entry.descriptor.supports_partial_acceptance =
        descriptor->supports_partial_acceptance != 0;
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

UmiStatus umi_editor_inline_suggestion_provider_registry_unregister(
    UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id)
{
    size_t position;

    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, provider_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
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

UmiStatus umi_editor_inline_suggestion_provider_registry_at(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    size_t position,
    UmiEditorInlineSuggestionProviderDescriptor *out_descriptor)
{
    if (registry == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->entries[position].descriptor;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_provider_registry_find(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    UmiEditorInlineSuggestionProviderDescriptor *out_descriptor)
{
    size_t position;

    if (registry == NULL || provider_id == NULL || out_descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, provider_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_descriptor = registry->entries[position].descriptor;
    return UMI_STATUS_OK;
}

int umi_editor_inline_suggestion_provider_supports_request(
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor,
    const UmiEditorCompletionRequest *request)
{
    if (validate_provider_descriptor(descriptor) != UMI_STATUS_OK ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK ||
        !descriptor->enabled ||
        (strcmp(descriptor->language_id, "*") != 0 &&
         strcmp(descriptor->language_id, request->language_id) != 0)) {
        return 0;
    }
    if (request->trigger_kind == UMI_EDITOR_COMPLETION_TRIGGER_INVOKED) {
        return descriptor->supports_manual_invocation;
    }
    return descriptor->supports_automatic_invocation;
}

UmiStatus umi_editor_inline_suggestion_provider_registry_invoke(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    const UmiEditorCompletionRequest *request,
    UmiEditorInlineSuggestionSink sink,
    void *sink_user_data,
    UmiEditorInlineSuggestionProviderResponse *out_response)
{
    size_t position;

    if (registry == NULL || provider_id == NULL || sink == NULL ||
        out_response == NULL ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, provider_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (!umi_editor_inline_suggestion_provider_supports_request(
            &registry->entries[position].descriptor, request)) {
        return UMI_STATUS_UNAVAILABLE;
    }
    if (umi_editor_completion_request_is_cancelled(request)) {
        return UMI_STATUS_CANCELLED;
    }
    (void)memset(out_response, 0, sizeof(*out_response));
    out_response->struct_size = (uint32_t)sizeof(*out_response);
    out_response->api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    return registry->entries[position].callbacks.provide(
        request,
        sink,
        sink_user_data,
        out_response,
        registry->entries[position].user_data);
}

UmiStatus umi_editor_inline_suggestion_provider_registry_cancel(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    const char *provider_id,
    uint64_t request_id)
{
    size_t position;

    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        request_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_provider_entry(registry, provider_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (registry->entries[position].callbacks.cancel == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return registry->entries[position].callbacks.cancel(
        request_id, registry->entries[position].user_data);
}

UmiStatus umi_editor_inline_suggestion_provider_registry_snapshot(
    const UmiEditorInlineSuggestionProviderRegistry *registry,
    UmiEditorInlineSuggestionProviderRegistrySnapshot *out_snapshot)
{
    size_t position;

    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    out_snapshot->provider_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (position = 0U; position < registry->count; ++position) {
        const UmiEditorInlineSuggestionProviderDescriptor *descriptor =
            &registry->entries[position].descriptor;
        if (descriptor->enabled) ++out_snapshot->enabled_provider_count;
        if (descriptor->supports_streaming) {
            ++out_snapshot->streaming_provider_count;
        }
        if (descriptor->source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
            ++out_snapshot->ai_provider_count;
        }
        if (descriptor->requires_network) {
            ++out_snapshot->remote_provider_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_inline_suggestion_provider_registry_count(
    const UmiEditorInlineSuggestionProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_editor_inline_suggestion_provider_registry_revision(
    const UmiEditorInlineSuggestionProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
