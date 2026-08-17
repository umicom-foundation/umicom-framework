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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static int valid_source_location(const UmiEditorSourceLocation *location)
{
    return location != NULL &&
           umi_editor_source_location_validate(location) == UMI_STATUS_OK &&
           terminated(location->uri, sizeof(location->uri)) &&
           terminated(location->label, sizeof(location->label)) &&
           terminated(location->symbol_id, sizeof(location->symbol_id)) &&
           terminated(location->preview, sizeof(location->preview));
}

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static int ascii_equal_n(const char *left,
                         size_t left_length,
                         const char *right,
                         size_t right_length)
{
    size_t index;

    if (left_length != right_length) return 0;
    for (index = 0U; index < left_length; ++index) {
        unsigned char left_character = (unsigned char)left[index];
        unsigned char right_character = (unsigned char)right[index];
        if (tolower(left_character) != tolower(right_character)) return 0;
    }
    return 1;
}

static int language_token_matches(const char *token,
                                  size_t token_length,
                                  const char *language_id)
{
    size_t language_length;

    if (token == NULL || language_id == NULL) return 0;
    language_length = strlen(language_id);
    if (token_length == 1U && token[0] == '*') return 1;
    if (token_length > 0U && token[token_length - 1U] == '*') {
        size_t prefix_length = token_length - 1U;
        return language_length >= prefix_length &&
               ascii_equal_n(token, prefix_length, language_id, prefix_length);
    }
    return ascii_equal_n(token, token_length, language_id, language_length);
}

static UmiStatus validate_descriptor(
    const UmiEditorNavigationProviderDescriptor *descriptor)
{
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

    if (functions == NULL ||
        functions->struct_size != (uint32_t)sizeof(*functions) ||
        functions->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((capabilities & query_capabilities) != 0U && functions->query == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((capabilities & symbol_capabilities) != 0U && functions->symbols == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((capabilities & hierarchy_capabilities) != 0U &&
        functions->hierarchy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((capabilities & UMI_EDITOR_NAVIGATION_CAPABILITY_SOURCE_PREVIEW) != 0U &&
        functions->preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus validate_registration(
    const UmiEditorNavigationProviderRegistration *registration)
{
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

static size_t find_provider(
    const UmiEditorNavigationProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;

    if (registry == NULL || provider_id == NULL) return SIZE_MAX;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].descriptor.id, provider_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static UmiStatus reserve_providers(
    UmiEditorNavigationProviderRegistry *registry,
    size_t required)
{
    size_t capacity;
    UmiEditorNavigationProviderRegistration *replacement;

    if (required <= registry->capacity) return UMI_STATUS_OK;
    if (required > UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = registry->capacity > 0U
        ? registry->capacity
        : UMI_EDITOR_NAVIGATION_PROVIDER_DEFAULT_CAPACITY;
    while (capacity < required) {
        if (capacity > UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY;
            break;
        }
        capacity *= 2U;
    }
    replacement = (UmiEditorNavigationProviderRegistration *)realloc(
        registry->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->items = replacement;
    registry->capacity = capacity;
    return UMI_STATUS_OK;
}

static int compare_provider(const void *left_pointer,
                            const void *right_pointer)
{
    const UmiEditorNavigationProviderRegistration *left =
        (const UmiEditorNavigationProviderRegistration *)left_pointer;
    const UmiEditorNavigationProviderRegistration *right =
        (const UmiEditorNavigationProviderRegistration *)right_pointer;

    if (left->descriptor.priority > right->descriptor.priority) return -1;
    if (left->descriptor.priority < right->descriptor.priority) return 1;
    return strcmp(left->descriptor.id, right->descriptor.id);
}

UmiStatus umi_editor_navigation_provider_descriptor_initialize(
    UmiEditorNavigationProviderDescriptor *descriptor,
    const char *provider_id,
    const char *label,
    const char *language_selector,
    UmiEditorNavigationProviderCapabilities capabilities,
    int32_t priority)
{
    UmiStatus status;

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
    if (status == UMI_STATUS_OK) {
        status = copy_text(descriptor->label, sizeof(descriptor->label), label);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(descriptor->language_selector,
                           sizeof(descriptor->language_selector),
                           language_selector != NULL && language_selector[0] != '\0'
                               ? language_selector
                               : "*");
    }
    if (status != UMI_STATUS_OK) (void)memset(descriptor, 0, sizeof(*descriptor));
    return status;
}

UmiStatus umi_editor_navigation_request_initialize(
    UmiEditorNavigationRequest *request,
    uint64_t request_id,
    UmiEditorNavigationQueryKind query_kind,
    const UmiEditorSourceLocation *origin,
    const char *language_id)
{
    UmiStatus status;

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
    if (status != UMI_STATUS_OK) (void)memset(request, 0, sizeof(*request));
    return status;
}

UmiStatus umi_editor_navigation_hierarchy_request_initialize(
    UmiEditorNavigationHierarchyRequest *request,
    uint64_t request_id,
    UmiEditorNavigationHierarchyKind kind,
    const char *root_symbol_id,
    const char *language_id)
{
    UmiStatus status;

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
    if (status == UMI_STATUS_OK) {
        status = copy_text(request->language_id, sizeof(request->language_id),
                           language_id != NULL ? language_id : "");
    }
    if (status != UMI_STATUS_OK) (void)memset(request, 0, sizeof(*request));
    return status;
}

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
    if (status == UMI_STATUS_OK) {
        status = copy_text(request->language_id, sizeof(request->language_id),
                           language_id != NULL ? language_id : "");
    }
    if (status != UMI_STATUS_OK) (void)memset(request, 0, sizeof(*request));
    return status;
}

UmiStatus umi_editor_navigation_provider_registry_create(
    UmiEditorNavigationProviderRegistry **out_registry)
{
    UmiEditorNavigationProviderRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiEditorNavigationProviderRegistry *)calloc(1U,
                                                              sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_editor_navigation_provider_registry_destroy(
    UmiEditorNavigationProviderRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    registry->items = NULL;
    free(registry);
}

UmiStatus umi_editor_navigation_provider_registry_clear(
    UmiEditorNavigationProviderRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    registry->count = 0U;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_provider_registry_upsert(
    UmiEditorNavigationProviderRegistry *registry,
    const UmiEditorNavigationProviderRegistration *registration)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || validate_registration(registration) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, registration->descriptor.id);
    if (index == SIZE_MAX) {
        status = reserve_providers(registry, registry->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = registry->count++;
    }
    registry->items[index] = *registration;
    if (registry->count > 1U) {
        qsort(registry->items, registry->count, sizeof(*registry->items),
              compare_provider);
    }
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_provider_registry_remove(
    UmiEditorNavigationProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;

    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, provider_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        (void)memmove(&registry->items[index], &registry->items[index + 1U],
                      (registry->count - index - 1U) * sizeof(*registry->items));
    }
    --registry->count;
    registry->revision = next_revision(registry->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_provider_registry_find(
    const UmiEditorNavigationProviderRegistry *registry,
    const char *provider_id,
    UmiEditorNavigationProviderRegistration *out_registration)
{
    size_t index;

    if (registry == NULL || provider_id == NULL || out_registration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_provider(registry, provider_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_registration = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_provider_registry_at(
    const UmiEditorNavigationProviderRegistry *registry,
    size_t position,
    UmiEditorNavigationProviderRegistration *out_registration)
{
    if (registry == NULL || out_registration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_registration = registry->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_provider_registry_snapshot(
    const UmiEditorNavigationProviderRegistry *registry,
    UmiEditorNavigationProviderRegistrySnapshot *out_snapshot)
{
    size_t index;

    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    out_snapshot->provider_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (index = 0U; index < registry->count; ++index) {
        UmiEditorNavigationProviderFlags flags =
            registry->items[index].descriptor.flags;
        if ((flags & UMI_EDITOR_NAVIGATION_PROVIDER_ENABLED) != 0U) {
            ++out_snapshot->enabled_count;
        }
        if ((flags & UMI_EDITOR_NAVIGATION_PROVIDER_REMOTE) != 0U) {
            ++out_snapshot->remote_count;
        }
        if ((flags & UMI_EDITOR_NAVIGATION_PROVIDER_CANCELLABLE) != 0U) {
            ++out_snapshot->cancellable_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_navigation_provider_registry_count(
    const UmiEditorNavigationProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_editor_navigation_provider_registry_revision(
    const UmiEditorNavigationProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

UmiEditorNavigationProviderCapabilities
umi_editor_navigation_capability_for_query_kind(
    UmiEditorNavigationQueryKind query_kind)
{
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

int umi_editor_navigation_provider_language_matches(
    const UmiEditorNavigationProviderDescriptor *descriptor,
    const char *language_id)
{
    const char *cursor;

    if (validate_descriptor(descriptor) != UMI_STATUS_OK) return 0;
    if (language_id == NULL || language_id[0] == '\0') return 1;
    cursor = descriptor->language_selector;
    while (*cursor != '\0') {
        const char *start;
        size_t length;
        while (*cursor == ' ' || *cursor == '\t' || *cursor == ',' ||
               *cursor == ';') {
            ++cursor;
        }
        start = cursor;
        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t' &&
               *cursor != ',' && *cursor != ';') {
            ++cursor;
        }
        length = (size_t)(cursor - start);
        if (length > 0U && language_token_matches(start, length, language_id)) {
            return 1;
        }
    }
    return 0;
}

int umi_editor_navigation_provider_supports_request(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationRequest *request)
{
    UmiEditorNavigationProviderCapabilities capability;

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

int umi_editor_navigation_provider_supports_hierarchy(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationHierarchyRequest *request)
{
    UmiEditorNavigationProviderCapabilities capability;

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

int umi_editor_navigation_provider_supports_preview(
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationPreviewRequest *request)
{
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
