/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/source_preview.h
 *
 * PURPOSE:
 *   Define a reusable live source-preview service for editor, designer,
 *   documentation and learning surfaces. The service selects a registered
 *   provider without exposing a graphical toolkit or renderer to applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROWSER_SOURCE_PREVIEW_H
#define UMICOM_BROWSER_SOURCE_PREVIEW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/browser/reading_document.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROWSER_SOURCE_PREVIEW_API_VERSION 1U
#define UMI_BROWSER_SOURCE_PREVIEW_PROVIDER_CAPACITY 32U
#define UMI_BROWSER_SOURCE_PREVIEW_ID_CAPACITY 128U
#define UMI_BROWSER_SOURCE_PREVIEW_PATH_CAPACITY 2048U
#define UMI_BROWSER_SOURCE_PREVIEW_SUMMARY_CAPACITY 512U
#define UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY 65536U

/** Describe the kind of result a frontend is being asked to present. */
typedef enum UmiBrowserSourcePreviewKind {
    UMI_BROWSER_SOURCE_PREVIEW_NONE = 0,
    UMI_BROWSER_SOURCE_PREVIEW_TEXT = 1,
    UMI_BROWSER_SOURCE_PREVIEW_READABLE_DOCUMENT = 2,
    UMI_BROWSER_SOURCE_PREVIEW_NATIVE_DOCUMENT = 3
} UmiBrowserSourcePreviewKind;

/**
 * Describe one immutable source revision. Source memory is borrowed only for
 * the duration of the render call, so providers must not retain its address.
 * source_length is an exact UTF-8 byte span, shorter than CONTENT_CAPACITY,
 * without embedded NUL; it need not have a trailing terminator. The service
 * supplies providers a terminated snapshot. Optional path/language strings
 * must terminate within PATH_CAPACITY / ID_CAPACITY and contain valid UTF-8.
 * Permission fields accept zero or one. Empty source is allowed with a nonnull
 * address; revision zero and a zero output budget are not valid requests.
 */
typedef struct UmiBrowserSourcePreviewRequest {
    uint32_t structure_size;
    uint32_t api_version;
    const char *path;
    const char *language_id;
    const char *source;
    size_t source_length;
    size_t maximum_output_bytes;
    uint64_t source_revision;
    int workspace_trusted;
    int allow_source_execution;
} UmiBrowserSourcePreviewRequest;

/**
 * Hold bounded output owned by the caller. Providers copy into this record so
 * no allocator or lifetime rule crosses a module boundary.
 */
typedef struct UmiBrowserSourcePreviewResult {
    uint32_t structure_size;
    uint32_t api_version;
    UmiBrowserSourcePreviewKind kind;
    char provider_id[UMI_BROWSER_SOURCE_PREVIEW_ID_CAPACITY];
    char title[UMI_BROWSER_SOURCE_PREVIEW_ID_CAPACITY];
    char summary[UMI_BROWSER_SOURCE_PREVIEW_SUMMARY_CAPACITY];
    char content[UMI_BROWSER_SOURCE_PREVIEW_CONTENT_CAPACITY];
    size_t source_bytes;
    size_t content_bytes;
    uint64_t source_revision;
    int truncated;
    int executed_source;
} UmiBrowserSourcePreviewResult;

/** Render one request through a provider-owned implementation. */
typedef UmiStatus (*UmiBrowserSourcePreviewRenderFn)(
    void *provider_context,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result);

/**
 * Describe provider matching and safety requirements. An empty language or
 * extension acts as a wildcard. Extensions include their leading full stop.
 */
typedef struct UmiBrowserSourcePreviewProvider {
    uint32_t structure_size;
    uint32_t api_version;
    char provider_id[UMI_BROWSER_SOURCE_PREVIEW_ID_CAPACITY];
    char title[UMI_BROWSER_SOURCE_PREVIEW_ID_CAPACITY];
    char language_id[UMI_BROWSER_SOURCE_PREVIEW_ID_CAPACITY];
    char extension[32U];
    uint32_t priority;
    int executes_source;
    int requires_trusted_workspace;
    UmiBrowserSourcePreviewRenderFn render;
    void *provider_context;
} UmiBrowserSourcePreviewProvider;

/** Own the provider catalogue while applications retain only an opaque handle. */
typedef struct UmiBrowserSourcePreviewService UmiBrowserSourcePreviewService;

/** Initialise a request with safe defaults and no permission to execute source. */
void umi_browser_source_preview_request_init(
    UmiBrowserSourcePreviewRequest *request);

/** Reset a reusable output record before a provider fills it. */
void umi_browser_source_preview_result_init(
    UmiBrowserSourcePreviewResult *result);

/** Initialise a provider descriptor before its matching fields are assigned. */
void umi_browser_source_preview_provider_init(
    UmiBrowserSourcePreviewProvider *provider);

/** Create an empty provider service. */
UmiStatus umi_browser_source_preview_service_create(
    UmiBrowserSourcePreviewService **out_service);

/** Release the provider catalogue without releasing caller-owned contexts. */
void umi_browser_source_preview_service_destroy(
    UmiBrowserSourcePreviewService *service);

/** Register a copied provider descriptor under its stable provider ID. */
UmiStatus umi_browser_source_preview_service_register(
    UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewProvider *provider);

/** Remove one provider while leaving all other registrations available. */
UmiStatus umi_browser_source_preview_service_unregister(
    UmiBrowserSourcePreviewService *service,
    const char *provider_id);

/** Install safe built-ins for readable HTML, the bounded Markdown profile and ordinary text documents.
 * On any refusal the complete prior catalogue remains unchanged. */
UmiStatus umi_browser_source_preview_service_install_builtins(
    UmiBrowserSourcePreviewService *service);

/** Resolve and invoke the highest-priority provider allowed by the request.
 * Every non-OK return leaves out_result byte-for-byte unchanged. On success,
 * output strings contain valid UTF-8 and built-in limits preserve scalars.
 * Executable providers require both workspace trust and explicit consent,
 * independently of their optional requires_trusted_workspace declaration.
 * Callers serialize catalogue access and keep provider contexts alive through
 * callbacks. A callback may unregister a provider, but must not destroy this
 * service. Capturing a descriptor preserves the in-flight provider identity.
 * A failure-atomic result does not roll back a custom provider's side effects.
 */
UmiStatus umi_browser_source_preview_service_render(
    const UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result);

/** Render text and optionally transfer a complete immutable reading model.
 * The ordinary provider selection, trust and failure contracts also apply.
 * Both outputs stay unchanged on failure. On success out_document is NULL for
 * providers without structured output or when the projected text was shortened.
 * The current structured providers are the built-in Markdown profile.
 * A nonnull document owns the same complete text as out_result.content; its
 * summary source_bytes counts the original source. Destroy it with
 * UmiBrowserReadingDocumentDestroy after all readers have finished.
 * Use a separate candidate pointer when replacing an existing owned model;
 * this service never releases a caller's previous document. Output objects
 * must be distinct and writable. No public result/provider layout is changed.
 */
UmiStatus umi_browser_source_preview_service_render_reading(
    const UmiBrowserSourcePreviewService *service,
    const UmiBrowserSourcePreviewRequest *request,
    UmiBrowserSourcePreviewResult *out_result,
    UmiBrowserReadingDocument **out_document);

/** Return the number of providers currently available to callers. */
size_t umi_browser_source_preview_service_count(
    const UmiBrowserSourcePreviewService *service);

/** Copy one provider descriptor for settings, diagnostics or documentation. */
UmiStatus umi_browser_source_preview_service_at(
    const UmiBrowserSourcePreviewService *service,
    size_t index,
    UmiBrowserSourcePreviewProvider *out_provider);

/** Return stable, user-facing text for a preview result kind. */
const char *umi_browser_source_preview_kind_text(
    UmiBrowserSourcePreviewKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_BROWSER_SOURCE_PREVIEW_H */
