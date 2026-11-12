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
#include "umicom/base/utf8.h"
#include "umicom/browser/reading_markdown.h"

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

/* Scalar-aware display copying replaces arbitrary byte truncation. The previous implementation is retained for engineering review. */
#if 0
/* Copy a display label up to the available capacity. Catalogue identifiers use
 * copy_text because truncating identity would be unsafe, while a long path or
 * document title may be shortened without changing provider behavior. */
static void copy_display_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    /* Without writable storage there is no display value to produce. */
    if (destination == NULL || capacity == 0U) return;
    /* A missing optional label is represented by a valid empty C string. */
    if (source == NULL) {
        destination[0] = '\0';
        return;
    }
    length = strlen(source);
    /* Leave one byte for the terminator when the label is shortened. */
    if (length >= capacity) length = capacity - 1U;
    (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

#endif

/* Bound scalar decoding by both the readable span and the output budget.
 * HTML's older byte-oriented extractor may leave an incomplete final scalar;
 * presentation stops before it instead of handing invalid text to a toolkit. */
static size_t preview_utf8_prefix(const char *text, size_t length, size_t budget)
{
    size_t offset = 0U;
    while (offset < length && offset < budget) {
        size_t used;
        uint32_t scalar;
        if (!UmiUtf8DecodeScalar((const unsigned char *)text + offset,
                                length - offset, &used, &scalar) ||
            used > budget - offset) break;
        offset += used;
    }
    return offset;
}

/* Optional metadata is a bounded C string; source itself is an exact byte span.
 * Stop at its first terminator, so a short string does not require capacity bytes. */
static int preview_utf8_text(const char *text, size_t capacity, int optional)
{
    size_t length;
    if (text == NULL) return optional;
    for (length = 0U; length < capacity; ++length) {
        if (text[length] == '\0')
            return UmiUtf8Validate((const unsigned char *)text, length, NULL);
    }
    return 0;
}

static void copy_display_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) { destination[0] = '\0'; return; }
    length = preview_utf8_prefix(source, strlen(source), capacity - 1U);
    (void)memcpy(destination, source, length);
    destination[length] = '\0';
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

/* Exact-span admission replaces the unbounded source strlen and rejects invalid display encoding. The previous implementation is retained for engineering review. */
#if 0
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

#endif

/* Admit bounded, non-NUL Unicode source before selecting any callback. A caller
 * owns source_length readable bytes; no byte beyond that span is inspected. */
static UmiStatus validate_request(const UmiBrowserSourcePreviewRequest *request)
{
    if (request == NULL ||
        request->structure_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_BROWSER_SOURCE_PREVIEW_API_VERSION ||
        request->source == NULL || request->source_revision == 0U ||
        request->source_length >= UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY ||
        request->maximum_output_bytes == 0U ||
        request->maximum_output_bytes >= UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY ||
        (request->workspace_trusted != 0 && request->workspace_trusted != 1) ||
        (request->allow_source_execution != 0 && request->allow_source_execution != 1) ||
        !preview_utf8_text(request->path, UMI_BROWSER_SOURCE_PREVIEW_PATH_CAPACITY, 1) ||
        !preview_utf8_text(request->language_id, UMI_BROWSER_SOURCE_PREVIEW_ID_CAPACITY, 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(request->source, '\0', request->source_length) != NULL ||
        !UmiUtf8Validate((const unsigned char *)request->source, request->source_length, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
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
    /* Catalogue strings can reach settings surfaces; validate them at the shared
     * boundary. Permission flags are canonical booleans, not arbitrary integers. */
    if (!preview_utf8_text(provider->provider_id, sizeof(provider->provider_id), 0) ||
        !preview_utf8_text(provider->title, sizeof(provider->title), 0) ||
        !preview_utf8_text(provider->language_id, sizeof(provider->language_id), 0) ||
        !preview_utf8_text(provider->extension, sizeof(provider->extension), 0) ||
        (provider->executes_source != 0 && provider->executes_source != 1) ||
        (provider->requires_trusted_workspace != 0 && provider->requires_trusted_workspace != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
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
    /* Callbacks fill a private candidate. Only complete, correctly encoded
     * results can cross into the frontend's previously visible result. */
    if (!preview_utf8_text(result->provider_id, sizeof(result->provider_id), 0) ||
        !preview_utf8_text(result->title, sizeof(result->title), 0) ||
        !preview_utf8_text(result->summary, sizeof(result->summary), 0) ||
        !UmiUtf8Validate((const unsigned char *)result->content, result->content_bytes, NULL) ||
        (result->truncated != 0 && result->truncated != 1) ||
        (result->executed_source != 0 && result->executed_source != 1))
        return UMI_STATUS_INVALID_STATE;
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
    /* The prior consent-only branch relied on every provider also declaring
     * its trust flag. Keep it for review; execution now always requires both. */
#if 0
    if (provider->executes_source && !request->allow_source_execution) {
#else
    if (provider->executes_source &&
        (!request->workspace_trusted || !request->allow_source_execution)) {
#endif
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
    /* Preserve scalar boundaries when the frontend requests a shorter preview.
     * The former byte-only limit is retained for comparison. */
#if 0
    if (length > request->maximum_output_bytes) {
        length = request->maximum_output_bytes;
        out_result->truncated = 1;
    }
#else
    length = preview_utf8_prefix(request->source, length, request->maximum_output_bytes);
    out_result->truncated = length < request->source_length;
#endif
    (void)memcpy(out_result->content, request->source, length);
    out_result->content[length] = '\0';
    out_result->kind = UMI_BROWSER_SOURCE_PREVIEW_TEXT;
    (void)copy_text(out_result->provider_id,
                    sizeof(out_result->provider_id),
                    "umicom.preview.plain-text");
    copy_display_text(out_result->title, sizeof(out_result->title),
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
    /* The extractor remains byte-oriented; the shared presentation service
     * publishes only complete UTF-8 scalars. Keep the old limit for review. */
#if 0
    if (length > request->maximum_output_bytes) {
        length = request->maximum_output_bytes;
        out_result->truncated = 1;
    }
#else
    length = preview_utf8_prefix(document.text, length, request->maximum_output_bytes);
    out_result->truncated = length < document.text_bytes;
#endif
    (void)memcpy(out_result->content, document.text, length);
    out_result->content[length] = '\0';
    out_result->kind = UMI_BROWSER_SOURCE_PREVIEW_READABLE_DOCUMENT;
    (void)copy_text(out_result->provider_id,
                    sizeof(out_result->provider_id),
                    "umicom.preview.readable-html");
    copy_display_text(out_result->title, sizeof(out_result->title),
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

/* Both HTML suffixes share extraction while retaining their registered identity. */
static UmiStatus render_readable_htm(
    void *provider_context, const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result)
{
    UmiStatus status = render_readable_html(provider_context, request, out_result);
    if (status != UMI_STATUS_OK) return status;
    return copy_text(out_result->provider_id, sizeof(out_result->provider_id),
                     "umicom.preview.readable-htm");
}

/* One Markdown projection supplies both text and optional native structure.
 * Shortened text cannot carry ranges for omitted content, so the owned model
 * is returned only when the complete projected text fits the requested budget. */
static UmiStatus render_markdown_document(const UmiBrowserSourcePreviewRequest *request,
    const char *identity, UmiBrowserSourcePreviewResult *result,
    UmiBrowserReadingDocument **out_document)
{
    UmiBrowserReadingDocument *document = NULL;
    UmiBrowserReadingSummary summary;
    UmiStatus status = UmiBrowserReadingMarkdownCreate(request->source,
        request->source_length, &document);
    if (status != UMI_STATUS_OK) return status;
    status = UmiBrowserReadingDocumentSummary(document, &summary);
    if (status != UMI_STATUS_OK) { UmiBrowserReadingDocumentDestroy(document); return status; }
    const char *text = UmiBrowserReadingDocumentText(document);
    size_t length = preview_utf8_prefix(text, summary.text_bytes, request->maximum_output_bytes);
    umi_browser_source_preview_result_init(result);
    result->kind = UMI_BROWSER_SOURCE_PREVIEW_READABLE_DOCUMENT;
    (void)copy_text(result->provider_id, sizeof(result->provider_id), identity);
    copy_display_text(result->title, sizeof(result->title),
        request->path != NULL && request->path[0] != '\0' ? request->path : "Markdown Preview");
    (void)copy_text(result->summary, sizeof(result->summary),
        "Bounded Markdown reading profile; raw HTML, links and images remain inert text.");
    memcpy(result->content, text, length); result->content[length] = '\0';
    result->content_bytes = length; result->source_bytes = request->source_length;
    result->source_revision = request->source_revision;
    result->truncated = length < summary.text_bytes;
    if (out_document != NULL && !result->truncated) *out_document = document;
    else UmiBrowserReadingDocumentDestroy(document);
    return UMI_STATUS_OK;
}
static UmiStatus render_readable_md(void *context,
    const UmiBrowserSourcePreviewRequest *request, UmiBrowserSourcePreviewResult *result)
{
    (void)context;
    return render_markdown_document(request, "umicom.preview.readable-md", result, NULL);
}
static UmiStatus render_readable_markdown(void *context,
    const UmiBrowserSourcePreviewRequest *request, UmiBrowserSourcePreviewResult *result)
{
    (void)context;
    return render_markdown_document(request, "umicom.preview.readable-markdown", result, NULL);
}
static UmiStatus render_markdown_language(void *context,
    const UmiBrowserSourcePreviewRequest *request, UmiBrowserSourcePreviewResult *result)
{
    (void)context;
    return render_markdown_document(request, "umicom.preview.markdown-language", result, NULL);
}

/* Shared provider construction preserves descriptor validation and priority
 * policy; no frontend owns a second Markdown parser or selector catalogue. */
static UmiStatus register_markdown_provider(UmiBrowserSourcePreviewService *service,
    const char *identity, const char *extension, const char *language,
    UmiBrowserSourcePreviewRenderFn render)
{
    UmiBrowserSourcePreviewProvider provider;
    umi_browser_source_preview_provider_init(&provider);
    (void)copy_text(provider.provider_id, sizeof(provider.provider_id), identity);
    (void)copy_text(provider.title, sizeof(provider.title), "Markdown Reading Preview");
    (void)copy_text(provider.extension, sizeof(provider.extension), extension);
    (void)copy_text(provider.language_id, sizeof(provider.language_id), language);
    provider.priority = 200U;
    provider.render = render;
    return umi_browser_source_preview_service_register(service, &provider);
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

/* The original three-provider catalogue is retained for review. The active
 * catalogue adds the shared Markdown profile while preserving its text fallback. */
#if 0
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
    /* Retain the previous shared callback assignment for review; the wrapper
     * now reports the htm identity required by result validation. */
#if 0
    provider.render = render_readable_html;
#else
    provider.render = render_readable_htm;
#endif
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

#endif

/* Retain incremental built-in registration for review. The active wrapper
 * stages the entire extension before publishing any new catalogue entries. */
#if 0
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
    /* Retain the previous shared callback assignment for review; the wrapper
     * now reports the htm identity required by result validation. */
#if 0
    provider.render = render_readable_html;
#else
    provider.render = render_readable_htm;
#endif
    status = umi_browser_source_preview_service_register(service, &provider);
    if (status != UMI_STATUS_OK) return status;

    status = register_markdown_provider(service, "umicom.preview.readable-md", ".md", "", render_readable_md);
    if (status != UMI_STATUS_OK) return status;
    status = register_markdown_provider(service, "umicom.preview.readable-markdown", ".markdown", "", render_readable_markdown);
    if (status != UMI_STATUS_OK) return status;
    status = register_markdown_provider(service, "umicom.preview.markdown-language", "", "markdown", render_markdown_language);
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

#endif

/* Install specific providers before the wildcard text fallback. */
static UmiStatus install_builtin_candidates(
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
    /* Retain the previous shared callback assignment for review; the wrapper
     * now reports the htm identity required by result validation. */
#if 0
    provider.render = render_readable_html;
#else
    provider.render = render_readable_htm;
#endif
    status = umi_browser_source_preview_service_register(service, &provider);
    if (status != UMI_STATUS_OK) return status;

    status = register_markdown_provider(service, "umicom.preview.readable-md", ".md", "", render_readable_md);
    if (status != UMI_STATUS_OK) return status;
    status = register_markdown_provider(service, "umicom.preview.readable-markdown", ".markdown", "", render_readable_markdown);
    if (status != UMI_STATUS_OK) return status;
    status = register_markdown_provider(service, "umicom.preview.markdown-language", "", "markdown", render_markdown_language);
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

/* Install a complete catalogue extension or preserve the prior catalogue.
 * Capacity or duplicate-ID refusal must not leave only some built-ins added. */
UmiStatus umi_browser_source_preview_service_install_builtins(
    UmiBrowserSourcePreviewService *service)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBrowserSourcePreviewService *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *service;
    UmiStatus status = install_builtin_candidates(candidate);
    if (status == UMI_STATUS_OK) *service = *candidate;
    free(candidate);
    return status;
}

/* Captured inputs and a private candidate replace direct writes to caller output; all failures preserve the old result. The previous implementation is retained for engineering review. */
#if 0
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

#endif

/* A render owns its input snapshot and unpublished result on the heap.
 * This also permits source/title metadata to refer to the caller's old result.
 * Provider contexts stay borrowed; callbacks must not destroy the service or
 * retain these addresses. Catalogue access is serialized by the caller. */
typedef struct UmiSourcePreviewOperation {
    UmiBrowserSourcePreviewRequest request;
    UmiBrowserSourcePreviewProvider provider;
    char source[UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY];
    char path[UMI_BROWSER_SOURCE_PREVIEW_PATH_CAPACITY];
    char language[UMI_BROWSER_SOURCE_PREVIEW_ID_CAPACITY];
    UmiBrowserSourcePreviewResult result;
} UmiSourcePreviewOperation;

/* Retain the text-only publication wrapper for review. The common owner below
 * can also transfer a complete reading model without a second parse. */
#if 0
UmiStatus umi_browser_source_preview_service_render(
    const UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result)
{
    const UmiBrowserSourcePreviewProvider *selected = NULL;
    UmiSourcePreviewOperation *operation;
    UmiStatus status;
    size_t index;
    if (service == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_request(request);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < service->count; ++index) {
        const UmiBrowserSourcePreviewProvider *candidate = &service->providers[index];
        if (provider_matches(candidate, request) && provider_is_allowed(candidate, request) &&
            (selected == NULL || candidate->priority > selected->priority))
            selected = candidate;
    }
    if (selected == NULL) return UMI_STATUS_NOT_FOUND;
    operation = (UmiSourcePreviewOperation *)malloc(sizeof(*operation));
    if (operation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    operation->request = *request;
    operation->provider = *selected;
    (void)memcpy(operation->source, request->source, request->source_length);
    operation->source[request->source_length] = '\0';
    operation->request.source = operation->source;
    if (request->path != NULL) {
        (void)copy_text(operation->path, sizeof(operation->path), request->path);
        operation->request.path = operation->path;
    }
    if (request->language_id != NULL) {
        (void)copy_text(operation->language, sizeof(operation->language), request->language_id);
        operation->request.language_id = operation->language;
    }
    umi_browser_source_preview_result_init(&operation->result);
    status = operation->provider.render(operation->provider.provider_context,
                                        &operation->request, &operation->result);
    if (status == UMI_STATUS_OK)
        status = validate_result(&operation->request, &operation->provider, &operation->result);
    if (status == UMI_STATUS_OK) *out_result = operation->result;
    free(operation);
    return status;
}

#endif

static UmiStatus source_preview_render_owned(
    const UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result,
    UmiBrowserReadingDocument **out_document)
{
    const UmiBrowserSourcePreviewProvider *selected = NULL;
    UmiSourcePreviewOperation *operation;
    UmiBrowserReadingDocument *document = NULL;
    UmiStatus status;
    size_t index;
    if (service == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_request(request);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < service->count; ++index) {
        const UmiBrowserSourcePreviewProvider *candidate = &service->providers[index];
        if (provider_matches(candidate, request) && provider_is_allowed(candidate, request) &&
            (selected == NULL || candidate->priority > selected->priority))
            selected = candidate;
    }
    if (selected == NULL) return UMI_STATUS_NOT_FOUND;
    operation = (UmiSourcePreviewOperation *)malloc(sizeof(*operation));
    if (operation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    operation->request = *request;
    operation->provider = *selected;
    (void)memcpy(operation->source, request->source, request->source_length);
    operation->source[request->source_length] = '\0';
    operation->request.source = operation->source;
    if (request->path != NULL) {
        (void)copy_text(operation->path, sizeof(operation->path), request->path);
        operation->request.path = operation->path;
    }
    if (request->language_id != NULL) {
        (void)copy_text(operation->language, sizeof(operation->language), request->language_id);
        operation->request.language_id = operation->language;
    }
    umi_browser_source_preview_result_init(&operation->result);
    if (out_document != NULL &&
        (operation->provider.render == render_readable_md ||
         operation->provider.render == render_readable_markdown ||
         operation->provider.render == render_markdown_language)) {
        const char *identity = operation->provider.render == render_readable_md
            ? "umicom.preview.readable-md" : operation->provider.render == render_readable_markdown
            ? "umicom.preview.readable-markdown" : "umicom.preview.markdown-language";
        status = render_markdown_document(&operation->request, identity,
            &operation->result, &document);
    } else {
        status = operation->provider.render(operation->provider.provider_context,
                                            &operation->request, &operation->result);
    }
    if (status == UMI_STATUS_OK)
        status = validate_result(&operation->request, &operation->provider, &operation->result);
    if (status == UMI_STATUS_OK) {
        *out_result = operation->result;
        if (out_document != NULL) { *out_document = document; document = NULL; }
    }
    UmiBrowserReadingDocumentDestroy(document);
    free(operation);
    return status;
}

/* Existing clients retain the same text result and public ABI. */
UmiStatus umi_browser_source_preview_service_render(
    const UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result)
{
    return source_preview_render_owned(service, request, out_result, NULL);
}

/* Both outputs are published together after the ordinary provider checks.
 * The caller owns a successful nonnull document and destroys it explicitly. */
UmiStatus umi_browser_source_preview_service_render_reading(
    const UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result,
    UmiBrowserReadingDocument **out_document)
{
    if (out_document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return source_preview_render_owned(service, request, out_result, out_document);
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
