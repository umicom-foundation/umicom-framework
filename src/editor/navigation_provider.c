/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_provider.c
 *
 * PURPOSE:
 *   Implement the provider registry and the validation/routing helpers used by
 *   the professional navigation platform.  The registry remains intentionally
 *   small and deterministic: descriptors are copied, priority order is stable,
 *   duplicate provider identities replace the prior registration, and no
 *   provider-owned object is destroyed by the Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/navigation_provider.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorNavigationProviderRegistry {
    UmiEditorNavigationProviderRegistration *items;
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

/*
 * Provide the valid source location operation used by this module and its client
 * applications.
 */
static int valid_source_location(const UmiEditorSourceLocation *location)
{
    return location != NULL &&
           umi_editor_source_location_validate(location) == UMI_STATUS_OK &&
           terminated(location->uri, sizeof(location->uri)) &&
           terminated(location->label, sizeof(location->label)) &&
           terminated(location->symbol_id, sizeof(location->symbol_id)) &&
           terminated(location->preview, sizeof(location->preview));
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the ascii equal n operation used by this module and its client applications. */
static int ascii_equal_n(const char *left,
                         size_t left_length,
                         const char *right,
                         size_t right_length)
{
    size_t index;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left_length != right_length) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < left_length; ++index) {
        unsigned char left_character = (unsigned char)left[index];
        unsigned char right_character = (unsigned char)right[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (tolower(left_character) != tolower(right_character)) return 0;
    }
    return 1;
}

/*
 * Provide the language token matches operation used by this module and its client
 * applications.
 */
static int language_token_matches(const char *token,
                                  size_t token_length,
                                  const char *language_id)
{
    size_t language_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (token == NULL || language_id == NULL) return 0;
    language_length = strlen(language_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token_length == 1U && token[0] == '*') return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token_length > 0U && token[token_length - 1U] == '*') {
        size_t prefix_length = token_length - 1U;
        return language_length >= prefix_length &&
               ascii_equal_n(token, prefix_length, language_id, prefix_length);
    }
    return ascii_equal_n(token, token_length, language_id, language_length);
}

/*
 * Provide the validate descriptor operation used by this module and its client
 * applications.
 */
static UmiStatus validate_descriptor(
    const UmiEditorNavigationProviderDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->struct_size != (uint32_t)sizeof(*descriptor) ||
        descriptor->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        !terminated(descriptor->id, sizeof(descriptor->id)) ||
        !terminated(descriptor->label, sizeof(descriptor->label)) ||
        !terminated(descriptor->language_selector,
                    sizeof(descriptor->language_selector)) ||
        descriptor->id[0] == '\0' || descriptor->label[0] == '\0' ||
        descriptor->language_selector[0] == '\0' ||
        (descriptor->capabilities &
         ~(UmiEditorNavigationProviderCapabilities)UMI_EDITOR_NAVIGATION_CAPABILITY_ALL) != 0U ||
        descriptor->capabilities == 0U ||
        (descriptor->flags & ~(UmiEditorNavigationProviderFlags)UMI_EDITOR_NAVIGATION_PROVIDER_ALL_FLAGS) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the validate functions operation used by this module and its client
 * applications.
 */
static UmiStatus validate_functions(
    const UmiEditorNavigationProviderFunctions *functions,
    UmiEditorNavigationProviderCapabilities capabilities)
{
    UmiEditorNavigationProviderCapabilities query_capabilities =
        UMI_EDITOR_NAVIGATION_CAPABILITY_DEFINITION |
        UMI_EDITOR_NAVIGATION_CAPABILITY_DECLARATION |
        UMI_EDITOR_NAVIGATION_CAPABILITY_TYPE_DEFINITION |
        UMI_EDITOR_NAVIGATION_CAPABILITY_IMPLEMENTATION |
        UMI_EDITOR_NAVIGATION_CAPABILITY_REFERENCES;
    UmiEditorNavigationProviderCapabilities symbol_capabilities =
        UMI_EDITOR_NAVIGATION_CAPABILITY_DOCUMENT_SYMBOLS |
        UMI_EDITOR_NAVIGATION_CAPABILITY_WORKSPACE_SYMBOLS;
    UmiEditorNavigationProviderCapabilities hierarchy_capabilities =
        UMI_EDITOR_NAVIGATION_CAPABILITY_CALL_HIERARCHY |
        UMI_EDITOR_NAVIGATION_CAPABILITY_TYPE_HIERARCHY;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (functions == NULL ||
        functions->struct_size != (uint32_t)sizeof(*functions) ||
        functions->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((capabilities & query_capabilities) != 0U && functions->query == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((capabilities & symbol_capabilities) != 0U && functions->symbols == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((capabilities & hierarchy_capabilities) != 0U &&
        functions->hierarchy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((capabilities & UMI_EDITOR_NAVIGATION_CAPABILITY_SOURCE_PREVIEW) != 0U &&
        functions->preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the validate registration operation used by this module and its client
 * applications.
 */
static UmiStatus validate_registration(
    const UmiEditorNavigationProviderRegistration *registration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registration == NULL ||
        registration->struct_size != (uint32_t)sizeof(*registration) ||
        registration->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        validate_descriptor(&registration->descriptor) != UMI_STATUS_OK ||
        validate_functions(&registration->functions,
                           registration->descriptor.capabilities) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the find provider operation used by this module and its client applications. */
static size_t find_provider(
    const UmiEditorNavigationProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].descriptor.id, provider_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Provide the reserve providers operation used by this module and its client applications. */
static UmiStatus reserve_providers(
    UmiEditorNavigationProviderRegistry *registry,
    size_t required)
{
    size_t capacity;
    UmiEditorNavigationProviderRegistration *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= registry->capacity) return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required > UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = registry->capacity > 0U
        ? registry->capacity
        : UMI_EDITOR_NAVIGATION_PROVIDER_DEFAULT_CAPACITY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY;
            break;
        }
        capacity *= 2U;
    }
    replacement = (UmiEditorNavigationProviderRegistration *)realloc(
        registry->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->items = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the compare provider operation used by this module and its client applications. */
static int compare_provider(const void *left_pointer,
                            const void *right_pointer)
{
    const UmiEditorNavigationProviderRegistration *left =
        (const UmiEditorNavigationProviderRegistration *)left_pointer;
    const UmiEditorNavigationProviderRegistration *right =
        (const UmiEditorNavigationProviderRegistration *)right_pointer;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->descriptor.priority > right->descriptor.priority) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->descriptor.priority < right->descriptor.priority) return 1;
    return strcmp(left->descriptor.id, right->descriptor.id);
}

/*
 * Initialise editor navigation provider descriptor from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_provider_descriptor_initialize(
    UmiEditorNavigationProviderDescriptor *descriptor,
    const char *provider_id,
    const char *label,
    const char *language_selector,
    UmiEditorNavigationProviderCapabilities capabilities,
    int32_t priority)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        label == NULL || label[0] == '\0' || capabilities == 0U ||
        (capabilities & ~(UmiEditorNavigationProviderCapabilities)UMI_EDITOR_NAVIGATION_CAPABILITY_ALL) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(descriptor, 0, sizeof(*descriptor));
    descriptor->struct_size = (uint32_t)sizeof(*descriptor);
    descriptor->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    descriptor->priority = priority;
    descriptor->capabilities = capabilities;
    descriptor->flags = UMI_EDITOR_NAVIGATION_PROVIDER_ENABLED;
    status = copy_text(descriptor->id, sizeof(descriptor->id), provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(descriptor->label, sizeof(descriptor->label), label);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(descriptor->language_selector,
                           sizeof(descriptor->language_selector),
                           language_selector != NULL && language_selector[0] != '\0'
                               ? language_selector
                               : "*");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) (void)memset(descriptor, 0, sizeof(*descriptor));
    return status;
}

/*
 * Initialise editor navigation request from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_navigation_request_initialize(
    UmiEditorNavigationRequest *request,
    uint64_t request_id,
    UmiEditorNavigationQueryKind query_kind,
    const UmiEditorSourceLocation *origin,
    const char *language_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || request_id == 0U ||
        query_kind <= UMI_EDITOR_NAVIGATION_QUERY_NONE ||
        query_kind > UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL ||
        !valid_source_location(origin)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    request->request_id = request_id;
    request->query_kind = query_kind;
    request->origin = *origin;
    request->document_revision = origin->document_revision;
    request->maximum_results = UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_RESULTS;
    status = copy_text(request->language_id, sizeof(request->language_id),
                       language_id != NULL ? language_id : "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) (void)memset(request, 0, sizeof(*request));
    return status;
}

/*
 * Initialise editor navigation hierarchy request from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_hierarchy_request_initialize(
    UmiEditorNavigationHierarchyRequest *request,
    uint64_t request_id,
    UmiEditorNavigationHierarchyKind kind,
    const char *root_symbol_id,
    const char *language_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || request_id == 0U || root_symbol_id == NULL ||
        root_symbol_id[0] == '\0' ||
        kind < UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING ||
        kind > UMI_EDITOR_NAVIGATION_HIERARCHY_TYPE_SUBTYPES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    request->request_id = request_id;
    request->kind = kind;
    request->maximum_depth = 8U;
    request->maximum_nodes =
        UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_HIERARCHY_NODES;
    request->include_indirect = 1;
    status = copy_text(request->root_symbol_id,
                       sizeof(request->root_symbol_id), root_symbol_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(request->language_id, sizeof(request->language_id),
                           language_id != NULL ? language_id : "");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) (void)memset(request, 0, sizeof(*request));
    return status;
}

/*
 * Initialise editor navigation preview request from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_preview_request_initialize(
    UmiEditorNavigationPreviewRequest *request,
    uint64_t request_id,
    const char *uri,
    const char *language_id,
    uint64_t document_revision,
    uint64_t start_line,
    uint64_t end_line)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || request_id == 0U || uri == NULL || uri[0] == '\0' ||
        end_line < start_line) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    request->request_id = request_id;
    request->document_revision = document_revision;
    request->start_line = start_line;
    request->end_line = end_line;
    request->maximum_bytes = UMI_EDITOR_NAVIGATION_PREVIEW_CONTENT_CAPACITY - 1U;
    status = copy_text(request->uri, sizeof(request->uri), uri);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(request->language_id, sizeof(request->language_id),
                           language_id != NULL ? language_id : "");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) (void)memset(request, 0, sizeof(*request));
    return status;
}

/*
 * Initialise editor navigation provider registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_provider_registry_create(
    UmiEditorNavigationProviderRegistry **out_registry)
{
    UmiEditorNavigationProviderRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorNavigationProviderRegistry *)calloc(1U,
                                                              sizeof(*registry));
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
 * Release or reset state held by editor navigation provider registry so the same storage
 * can be reused safely.
 */
void umi_editor_navigation_provider_registry_destroy(
    UmiEditorNavigationProviderRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    free(registry->items);
    registry->items = NULL;
    free(registry);
}

/*
 * Release or reset state held by editor navigation provider registry so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_navigation_provider_registry_clear(
    UmiEditorNavigationProviderRegistry *registry)
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
 * Provide the editor navigation provider registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_provider_registry_upsert(
    UmiEditorNavigationProviderRegistry *registry,
    const UmiEditorNavigationProviderRegistration *registration)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || validate_registration(registration) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, registration->descriptor.id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        status = reserve_providers(registry, registry->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        index = registry->count++;
    }
    registry->items[index] = *registration;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count > 1U) {
        qsort(registry->items, registry->count, sizeof(*registry->items),
              compare_provider);
    }
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove editor navigation provider registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_editor_navigation_provider_registry_remove(
    UmiEditorNavigationProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, provider_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        (void)memmove(&registry->items[index], &registry->items[index + 1U],
                      (registry->count - index - 1U) * sizeof(*registry->items));
    }
    --registry->count;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor navigation provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_navigation_provider_registry_find(
    const UmiEditorNavigationProviderRegistry *registry,
    const char *provider_id,
    UmiEditorNavigationProviderRegistration *out_registration)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || out_registration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, provider_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_registration = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find editor navigation provider registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_navigation_provider_registry_at(
    const UmiEditorNavigationProviderRegistry *registry,
    size_t position,
    UmiEditorNavigationProviderRegistration *out_registration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_registration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_registration = registry->items[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor navigation provider registry snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_provider_registry_snapshot(
    const UmiEditorNavigationProviderRegistry *registry,
    UmiEditorNavigationProviderRegistrySnapshot *out_snapshot)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    out_snapshot->provider_count = registry->count;
    out_snapshot->revision = registry->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        UmiEditorNavigationProviderFlags flags =
            registry->items[index].descriptor.flags;
        /* Apply this operation only while the related capability or state is available. */
        if ((flags & UMI_EDITOR_NAVIGATION_PROVIDER_ENABLED) != 0U) {
            ++out_snapshot->enabled_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((flags & UMI_EDITOR_NAVIGATION_PROVIDER_REMOTE) != 0U) {
            ++out_snapshot->remote_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((flags & UMI_EDITOR_NAVIGATION_PROVIDER_CANCELLABLE) != 0U) {
            ++out_snapshot->cancellable_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor navigation provider registry without
 * changing their state.
 */
size_t umi_editor_navigation_provider_registry_count(
    const UmiEditorNavigationProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the editor navigation provider registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_navigation_provider_registry_revision(
    const UmiEditorNavigationProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/*
 * Provide the editor navigation capability for query kind operation used by this module
 * and its client applications.
 */
UmiEditorNavigationProviderCapabilities
umi_editor_navigation_capability_for_query_kind(
    UmiEditorNavigationQueryKind query_kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (query_kind) {
        case UMI_EDITOR_NAVIGATION_QUERY_DEFINITION:
            return UMI_EDITOR_NAVIGATION_CAPABILITY_DEFINITION;
        case UMI_EDITOR_NAVIGATION_QUERY_DECLARATION:
            return UMI_EDITOR_NAVIGATION_CAPABILITY_DECLARATION;
        case UMI_EDITOR_NAVIGATION_QUERY_REFERENCE:
            return UMI_EDITOR_NAVIGATION_CAPABILITY_REFERENCES;
        case UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION:
            return UMI_EDITOR_NAVIGATION_CAPABILITY_IMPLEMENTATION;
        case UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION:
            return UMI_EDITOR_NAVIGATION_CAPABILITY_TYPE_DEFINITION;
        case UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL:
            return UMI_EDITOR_NAVIGATION_CAPABILITY_DOCUMENT_SYMBOLS;
        case UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL:
            return UMI_EDITOR_NAVIGATION_CAPABILITY_WORKSPACE_SYMBOLS;
        case UMI_EDITOR_NAVIGATION_QUERY_NONE:
        default:
            return 0U;
    }
}

/*
 * Provide the editor navigation provider language matches operation used by this module
 * and its client applications.
 */
int umi_editor_navigation_provider_language_matches(
    const UmiEditorNavigationProviderDescriptor *descriptor,
    const char *language_id)
{
    const char *cursor;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (validate_descriptor(descriptor) != UMI_STATUS_OK) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (language_id == NULL || language_id[0] == '\0') return 1;
    cursor = descriptor->language_selector;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *start;
        size_t length;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor == ' ' || *cursor == '\t' || *cursor == ',' ||
               *cursor == ';') {
            ++cursor;
        }
        start = cursor;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' &&
               *cursor != ',' && *cursor != ';') {
            ++cursor;
        }
        length = (size_t)(cursor - start);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U && language_token_matches(start, length, language_id)) {
            return 1;
        }
    }
    return 0;
}

/*
 * Provide the editor navigation provider supports request operation used by this module
 * and its client applications.
 */
int umi_editor_navigation_provider_supports_request(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationRequest *request)
{
    UmiEditorNavigationProviderCapabilities capability;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (validate_registration(registration) != UMI_STATUS_OK || request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        request->request_id == 0U ||
        request->query_kind <= UMI_EDITOR_NAVIGATION_QUERY_NONE ||
        request->query_kind > UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL ||
        request->maximum_results > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS ||
        (request->flags &
         ~(UmiEditorNavigationRequestFlags)UMI_EDITOR_NAVIGATION_REQUEST_ALL_FLAGS) != 0U ||
        !valid_source_location(&request->origin) ||
        !terminated(request->workspace_id, sizeof(request->workspace_id)) ||
        !terminated(request->document_id, sizeof(request->document_id)) ||
        !terminated(request->language_id, sizeof(request->language_id)) ||
        !terminated(request->symbol_id, sizeof(request->symbol_id)) ||
        !terminated(request->text, sizeof(request->text))) {
        return 0;
    }
    capability = umi_editor_navigation_capability_for_query_kind(
        request->query_kind);
    return capability != 0U &&
           (registration->descriptor.flags &
            UMI_EDITOR_NAVIGATION_PROVIDER_ENABLED) != 0U &&
           (registration->descriptor.capabilities & capability) != 0U &&
           umi_editor_navigation_provider_language_matches(
               &registration->descriptor, request->language_id);
}

/*
 * Provide the editor navigation provider supports hierarchy operation used by this module
 * and its client applications.
 */
int umi_editor_navigation_provider_supports_hierarchy(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationHierarchyRequest *request)
{
    UmiEditorNavigationProviderCapabilities capability;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (validate_registration(registration) != UMI_STATUS_OK || request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        request->request_id == 0U || request->root_symbol_id[0] == '\0' ||
        request->kind < UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING ||
        request->kind > UMI_EDITOR_NAVIGATION_HIERARCHY_TYPE_SUBTYPES ||
        request->maximum_depth == 0U ||
        request->maximum_depth >
            UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES ||
        request->maximum_nodes == 0U ||
        request->maximum_nodes > UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES ||
        !terminated(request->root_symbol_id, sizeof(request->root_symbol_id)) ||
        !terminated(request->expand_node_id, sizeof(request->expand_node_id)) ||
        !terminated(request->language_id, sizeof(request->language_id))) {
        return 0;
    }
    capability = request->kind == UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING ||
                         request->kind == UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_OUTGOING
        ? UMI_EDITOR_NAVIGATION_CAPABILITY_CALL_HIERARCHY
        : UMI_EDITOR_NAVIGATION_CAPABILITY_TYPE_HIERARCHY;
    return (registration->descriptor.flags &
            UMI_EDITOR_NAVIGATION_PROVIDER_ENABLED) != 0U &&
           (registration->descriptor.capabilities & capability) != 0U &&
           umi_editor_navigation_provider_language_matches(
               &registration->descriptor, request->language_id);
}

/*
 * Provide the editor navigation provider supports preview operation used by this module
 * and its client applications.
 */
int umi_editor_navigation_provider_supports_preview(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationPreviewRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (validate_registration(registration) != UMI_STATUS_OK || request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        request->request_id == 0U || request->uri[0] == '\0' ||
        request->end_line < request->start_line ||
        request->maximum_bytes == 0U ||
        request->maximum_bytes >= UMI_EDITOR_NAVIGATION_PREVIEW_CONTENT_CAPACITY ||
        !terminated(request->uri, sizeof(request->uri)) ||
        !terminated(request->language_id, sizeof(request->language_id))) {
        return 0;
    }
    return (registration->descriptor.flags &
            UMI_EDITOR_NAVIGATION_PROVIDER_ENABLED) != 0U &&
           (registration->descriptor.capabilities &
            UMI_EDITOR_NAVIGATION_CAPABILITY_SOURCE_PREVIEW) != 0U &&
           umi_editor_navigation_provider_language_matches(
               &registration->descriptor, request->language_id);
}
