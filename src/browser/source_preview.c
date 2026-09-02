/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/source_preview.c
 *
 * PURPOSE:
 *   Select and safely run source-preview providers. Built-in providers never
 *   execute source; adapters that do must pass both trust and explicit consent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/browser/source_preview.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/browser/document_preview.h"

struct UmiBrowserSourcePreviewService {
    UmiBrowserSourcePreviewProvider providers[
        UMI_BROWSER_SOURCE_PREVIEW_PROVIDER_CAPACITY];
    size_t count;
};

/* Report whether a bounded character array contains a terminating null byte. */
static int text_is_terminated(const char *text, size_t capacity)
{
    return text != NULL && capacity > 0U &&
           memchr(text, '\0', capacity) != NULL;
}

/* Copy text only when the complete value and its terminator fit. */
static UmiStatus copy_text(
    char *destination,
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

/* Compare ASCII identifiers without applying locale-dependent case rules. */
static int text_equals_casefold(const char *left, const char *right)
{
    if (left == NULL || right == NULL) return 0;
    while (*left != '\0' && *right != '\0') {
        if (tolower((unsigned char)*left) !=
            tolower((unsigned char)*right)) {
            return 0;
        }
        ++left;
        ++right;
    }
    return *left == '\0' && *right == '\0';
}

/* Return the final file extension, or an empty string when none is present. */
static const char *path_extension(const char *path)
{
    const char *extension;
    const char *slash;
    const char *backslash;

    if (path == NULL) return "";
    extension = strrchr(path, '.');
    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    if (extension == NULL || (slash != NULL && extension < slash) ||
        (backslash != NULL && extension < backslash)) {
        return "";
    }
    return extension;
}

/* Validate borrowed pointers and bounds before any provider sees the request. */
static UmiStatus validate_request(
    const UmiBrowserSourcePreviewRequest *request)
{
    if (request == NULL ||
        request->structure_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_BROWSER_SOURCE_PREVIEW_API_VERSION ||
        request->source == NULL || request->source_revision == 0U ||
        request->source_length != strlen(request->source) ||
        request->source_length >= UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY ||
        request->maximum_output_bytes == 0U ||
        request->maximum_output_bytes >=
            UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Reject malformed descriptors before they enter the copied catalogue. */
static UmiStatus validate_provider(
    const UmiBrowserSourcePreviewProvider *provider)
{
    if (provider == NULL ||
        provider->structure_size != (uint32_t)sizeof(*provider) ||
        provider->api_version != UMI_BROWSER_SOURCE_PREVIEW_API_VERSION ||
        !text_is_terminated(provider->provider_id,
                            sizeof(provider->provider_id)) ||
        !text_is_terminated(provider->title, sizeof(provider->title)) ||
        !text_is_terminated(provider->language_id,
                            sizeof(provider->language_id)) ||
        !text_is_terminated(provider->extension,
                            sizeof(provider->extension)) ||
        provider->provider_id[0] == '\0' || provider->title[0] == '\0' ||
        provider->render == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Extension selectors always include a leading full stop, making suffix
     * matching unambiguous for filenames such as archive.tar and page.html. */
    if (provider->extension[0] != '\0' && provider->extension[0] != '.') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Confirm that provider output still describes the requested source revision. */
static UmiStatus validate_result(
    const UmiBrowserSourcePreviewRequest *request,
    const UmiBrowserSourcePreviewProvider *provider,
    const UmiBrowserSourcePreviewResult *result)
{
    if (result == NULL ||
        result->structure_size != (uint32_t)sizeof(*result) ||
        result->api_version != UMI_BROWSER_SOURCE_PREVIEW_API_VERSION ||
        result->kind <= UMI_BROWSER_SOURCE_PREVIEW_NONE ||
        result->kind > UMI_BROWSER_SOURCE_PREVIEW_NATIVE_DOCUMENT ||
        !text_is_terminated(result->provider_id,
                            sizeof(result->provider_id)) ||
        !text_is_terminated(result->title, sizeof(result->title)) ||
        !text_is_terminated(result->summary, sizeof(result->summary)) ||
        !text_is_terminated(result->content, sizeof(result->content)) ||
        strcmp(result->provider_id, provider->provider_id) != 0 ||
        result->source_revision != request->source_revision ||
        result->source_bytes != request->source_length ||
        result->content_bytes != strlen(result->content) ||
        result->content_bytes > request->maximum_output_bytes ||
        (result->executed_source != 0) != (provider->executes_source != 0)) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Match both configured selectors. Empty selectors deliberately match all. */
static int provider_matches(
    const UmiBrowserSourcePreviewProvider *provider,
    const UmiBrowserSourcePreviewRequest *request)
{
    const char *extension = path_extension(request->path);
    int language_matches = provider->language_id[0] == '\0' ||
        text_equals_casefold(provider->language_id, request->language_id);
    int extension_matches = provider->extension[0] == '\0' ||
        text_equals_casefold(provider->extension, extension);

    return language_matches && extension_matches;
}

/* Explain whether the request grants every permission a provider declares. */
static int provider_is_allowed(
    const UmiBrowserSourcePreviewProvider *provider,
    const UmiBrowserSourcePreviewRequest *request)
{
    if (provider->requires_trusted_workspace && !request->workspace_trusted) {
        return 0;
    }
    if (provider->executes_source && !request->allow_source_execution) {
        return 0;
    }
    return 1;
}

/* Copy plain source into a read-only preview without executing or interpreting it. */
static UmiStatus render_plain_text(
    void *provider_context,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result)
{
    size_t length = request->source_length;
    (void)provider_context;

    umi_browser_source_preview_result_init(out_result);
    if (length > request->maximum_output_bytes) {
        length = request->maximum_output_bytes;
        out_result->truncated = 1;
    }
    (void)memcpy(out_result->content, request->source, length);
    out_result->content[length] = '\0';
    out_result->kind = UMI_BROWSER_SOURCE_PREVIEW_TEXT;
    (void)copy_text(out_result->provider_id,
                    sizeof(out_result->provider_id),
                    "umicom.preview.plain-text");
    (void)copy_text(out_result->title, sizeof(out_result->title),
                    request->path != NULL && request->path[0] != '\0'
                        ? request->path
                        : "Text Preview");
    (void)copy_text(out_result->summary, sizeof(out_result->summary),
                    "Read-only source preview; no source was executed.");
    out_result->source_bytes = request->source_length;
    out_result->content_bytes = length;
    out_result->source_revision = request->source_revision;
    return UMI_STATUS_OK;
}

/* Convert HTML to readable text while removing markup, style and script content. */
static UmiStatus render_readable_html(
    void *provider_context,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result)
{
    UmiBrowserDocumentPreview document;
    size_t length;
    UmiStatus status;
    (void)provider_context;

    status = umi_browser_document_preview_from_html(
        request->source, request->source_length, &document);
    if (status != UMI_STATUS_OK) return status;

    umi_browser_source_preview_result_init(out_result);
    length = document.text_bytes;
    if (length > request->maximum_output_bytes) {
        length = request->maximum_output_bytes;
        out_result->truncated = 1;
    }
    (void)memcpy(out_result->content, document.text, length);
    out_result->content[length] = '\0';
    out_result->kind = UMI_BROWSER_SOURCE_PREVIEW_READABLE_DOCUMENT;
    (void)copy_text(out_result->provider_id,
                    sizeof(out_result->provider_id),
                    "umicom.preview.readable-html");
    (void)copy_text(out_result->title, sizeof(out_result->title),
                    document.title[0] != '\0'
                        ? document.title
                        : "HTML Preview");
    (void)copy_text(out_result->summary, sizeof(out_result->summary),
                    "Readable document preview; scripts and styles were not executed.");
    out_result->source_bytes = request->source_length;
    out_result->content_bytes = length;
    out_result->source_revision = request->source_revision;
    out_result->truncated = out_result->truncated || document.truncated;
    return UMI_STATUS_OK;
}

/* Fill every versioned field so callers can safely reuse stack records. */
void umi_browser_source_preview_request_init(
    UmiBrowserSourcePreviewRequest *request)
{
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->structure_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_BROWSER_SOURCE_PREVIEW_API_VERSION;
    request->maximum_output_bytes =
        UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY - 1U;
}

/* Clear earlier content before a provider writes a new result. */
void umi_browser_source_preview_result_init(
    UmiBrowserSourcePreviewResult *result)
{
    if (result == NULL) return;
    (void)memset(result, 0, sizeof(*result));
    result->structure_size = (uint32_t)sizeof(*result);
    result->api_version = UMI_BROWSER_SOURCE_PREVIEW_API_VERSION;
}

/* Initialise a descriptor with the current public ABI version. */
void umi_browser_source_preview_provider_init(
    UmiBrowserSourcePreviewProvider *provider)
{
    if (provider == NULL) return;
    (void)memset(provider, 0, sizeof(*provider));
    provider->structure_size = (uint32_t)sizeof(*provider);
    provider->api_version = UMI_BROWSER_SOURCE_PREVIEW_API_VERSION;
}

/* Allocate the small fixed-capacity catalogue on the heap. */
UmiStatus umi_browser_source_preview_service_create(
    UmiBrowserSourcePreviewService **out_service)
{
    UmiBrowserSourcePreviewService *service;

    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiBrowserSourcePreviewService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *out_service = service;
    return UMI_STATUS_OK;
}

/* Provider contexts remain caller-owned because the descriptor has no destructor. */
void umi_browser_source_preview_service_destroy(
    UmiBrowserSourcePreviewService *service)
{
    free(service);
}

/* Copy a valid unique provider into the first free catalogue slot. */
UmiStatus umi_browser_source_preview_service_register(
    UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewProvider *provider)
{
    size_t index;
    UmiStatus status = validate_provider(provider);

    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < service->count; ++index) {
        if (strcmp(service->providers[index].provider_id,
                   provider->provider_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (service->count >= UMI_BROWSER_SOURCE_PREVIEW_PROVIDER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    service->providers[service->count++] = *provider;
    return UMI_STATUS_OK;
}

/* Compact the copied array after removing the requested stable identifier. */
UmiStatus umi_browser_source_preview_service_unregister(
    UmiBrowserSourcePreviewService *service,
    const char *provider_id)
{
    size_t index;

    if (service == NULL || provider_id == NULL || provider_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < service->count; ++index) {
        if (strcmp(service->providers[index].provider_id, provider_id) == 0) {
            size_t remaining = service->count - index - 1U;
            if (remaining > 0U) {
                (void)memmove(&service->providers[index],
                              &service->providers[index + 1U],
                              remaining * sizeof(service->providers[0]));
            }
            --service->count;
            (void)memset(&service->providers[service->count], 0,
                         sizeof(service->providers[0]));
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Install specific providers before the wildcard text fallback. */
UmiStatus umi_browser_source_preview_service_install_builtins(
    UmiBrowserSourcePreviewService *service)
{
    UmiBrowserSourcePreviewProvider provider;
    UmiStatus status;

    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_browser_source_preview_provider_init(&provider);
    (void)copy_text(provider.provider_id, sizeof(provider.provider_id),
                    "umicom.preview.readable-html");
    (void)copy_text(provider.title, sizeof(provider.title),
                    "Readable HTML Preview");
    (void)copy_text(provider.extension, sizeof(provider.extension), ".html");
    provider.priority = 200U;
    provider.render = render_readable_html;
    status = umi_browser_source_preview_service_register(service, &provider);
    if (status != UMI_STATUS_OK) return status;

    umi_browser_source_preview_provider_init(&provider);
    (void)copy_text(provider.provider_id, sizeof(provider.provider_id),
                    "umicom.preview.readable-htm");
    (void)copy_text(provider.title, sizeof(provider.title),
                    "Readable HTML Preview");
    (void)copy_text(provider.extension, sizeof(provider.extension), ".htm");
    provider.priority = 200U;
    provider.render = render_readable_html;
    status = umi_browser_source_preview_service_register(service, &provider);
    if (status != UMI_STATUS_OK) return status;

    umi_browser_source_preview_provider_init(&provider);
    (void)copy_text(provider.provider_id, sizeof(provider.provider_id),
                    "umicom.preview.plain-text");
    (void)copy_text(provider.title, sizeof(provider.title),
                    "Plain Text Preview");
    /* An empty selector is the safe fallback for every source family. More
     * capable providers win through their greater priority. */
    provider.priority = 100U;
    provider.render = render_plain_text;
    return umi_browser_source_preview_service_register(service, &provider);
}

/* Select the allowed matching provider with the greatest declared priority. */
UmiStatus umi_browser_source_preview_service_render(
    const UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result)
{
    const UmiBrowserSourcePreviewProvider *selected = NULL;
    size_t index;
    UmiStatus status;

    if (service == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = validate_request(request);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < service->count; ++index) {
        const UmiBrowserSourcePreviewProvider *candidate =
            &service->providers[index];
        if (provider_matches(candidate, request) &&
            provider_is_allowed(candidate, request) &&
            (selected == NULL || candidate->priority > selected->priority)) {
            selected = candidate;
        }
    }
    if (selected == NULL) return UMI_STATUS_NOT_FOUND;

    umi_browser_source_preview_result_init(out_result);
    status = selected->render(
        selected->provider_context, request, out_result);
    if (status != UMI_STATUS_OK) return status;
    return validate_result(request, selected, out_result);
}

/* Expose catalogue size without allowing callers to mutate the internal array. */
size_t umi_browser_source_preview_service_count(
    const UmiBrowserSourcePreviewService *service)
{
    return service != NULL ? service->count : 0U;
}

/* Copy one descriptor so diagnostics never borrow an internal array address. */
UmiStatus umi_browser_source_preview_service_at(
    const UmiBrowserSourcePreviewService *service,
    size_t index,
    UmiBrowserSourcePreviewProvider *out_provider)
{
    if (service == NULL || out_provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= service->count) return UMI_STATUS_NOT_FOUND;
    *out_provider = service->providers[index];
    return UMI_STATUS_OK;
}

/* Provide short labels that every frontend can display consistently. */
const char *umi_browser_source_preview_kind_text(
    UmiBrowserSourcePreviewKind kind)
{
    switch (kind) {
        case UMI_BROWSER_SOURCE_PREVIEW_NONE: return "none";
        case UMI_BROWSER_SOURCE_PREVIEW_TEXT: return "text";
        case UMI_BROWSER_SOURCE_PREVIEW_READABLE_DOCUMENT:
            return "readable document";
        case UMI_BROWSER_SOURCE_PREVIEW_NATIVE_DOCUMENT:
            return "native document";
        default: return "unknown";
    }
}
