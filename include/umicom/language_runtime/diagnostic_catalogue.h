/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/diagnostic_catalogue.h
 * PURPOSE: Own a complete language-server diagnostic publication and its related locations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_CATALOGUE_H
#define UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_CATALOGUE_H
#include "umicom/language_runtime/location_catalogue.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageDiagnostic
    {
        UmiLanguageSourceRange range;
        const char *message, *source, *code, *description_uri;
        int severity, has_severity, has_code, code_is_number, unnecessary, deprecated, has_data;
        size_t related_count;
    } UmiLanguageDiagnostic;
    typedef struct UmiLanguageDiagnosticRelated
    {
        UmiLanguageSourceLocation location;
        const char *message;
    } UmiLanguageDiagnosticRelated;
    typedef struct UmiLanguageDiagnosticPublication
    {
        const char *uri;
        int has_version;
        int32_t version;
    } UmiLanguageDiagnosticPublication;
    typedef struct UmiLanguageDiagnosticCatalogue UmiLanguageDiagnosticCatalogue;
    /* Read a complete publishDiagnostics parameters object. Each publication is a
 * replacement set, including an empty array; never merge old and new rows.
 * Own messages, source labels, numeric/string codes, explanation URIs, related
 * locations and the original JSON (including opaque data). Do not open links,
 * execute fixes or read files. Missing severity is presented as Error, while
 * has_severity records its absence. Unknown positive tags are retained in the
 * original JSON and ignored by the known-tag flags.
 *
 * Bounds: 1 MiB JSON, 4096 diagnostics, 128 related rows per diagnostic and
 * 8192 related rows overall; 65536 bytes per message, 4096 per source/code and
 * 8192 per URI. Known members must have valid types and occur once. Any invalid
 * row or exceeded limit rejects the whole publication. Failure clears output.
 * Cancellation is observed between rows and during JSON parsing. */
    UmiStatus UmiLanguageDiagnosticCatalogueCreate(const void *json, size_t bytes,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageDiagnosticCatalogue **out_catalogue);
    /* Require a JSON-RPC notification with this exact method, no id/result/error,
 * and a complete parameters object. A different valid method returns NOT_FOUND. */
    UmiStatus UmiLanguageDiagnosticCatalogueReadNotification(const void *json, size_t bytes,
                                                             const UmiCancellationToken *cancel,
                                                             UmiLanguageDiagnosticCatalogue **out_catalogue);
    /* Read a correlated textDocument/diagnostic response for the supplied URI.
     * A fresh request has no previous result: require a full report with items,
     * never interpret an unchanged report as an empty diagnostic set. Retain
     * the complete response JSON, opaque diagnostic data and optional result ID.
     * Nonempty relatedDocuments is refused rather than publishing a subset;
     * hosts using this reader must advertise relatedDocumentSupport false.
     * The URI is copied, and no document version is invented. The existing
     * catalogue bounds and source-validation rules apply. Failure clears output. */
    UmiStatus UmiLanguageDiagnosticCatalogueReadPullResponse(const void *json, size_t bytes,
                                                             uint64_t expected_request_id,
                                                             const char *document_uri,
                                                             const UmiCancellationToken *cancel,
                                                             UmiLanguageDiagnosticCatalogue **out_catalogue);
    /* Borrow an optional pull result ID until Destroy. NOT_FOUND means no ID;
     * failure clears output. Reusing it requires the same diagnostic provider
     * and a retained previous report; temporary native queries do not reuse it. */
    UmiStatus UmiLanguageDiagnosticCataloguePullResultId(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                         const char **out_result_id);
    void UmiLanguageDiagnosticCatalogueDestroy(UmiLanguageDiagnosticCatalogue *catalogue);
    size_t UmiLanguageDiagnosticCatalogueCount(const UmiLanguageDiagnosticCatalogue *catalogue);
    /* Metadata is copied and strings remain borrowed until Destroy. Invalid
 * arguments or indices leave metadata output unchanged. */
    UmiStatus UmiLanguageDiagnosticCataloguePublication(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                        UmiLanguageDiagnosticPublication *out_publication);
    UmiStatus UmiLanguageDiagnosticCatalogueAt(const UmiLanguageDiagnosticCatalogue *catalogue, size_t index,
                                               UmiLanguageDiagnostic *out_diagnostic);
    UmiStatus UmiLanguageDiagnosticCatalogueRelated(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                    size_t index, size_t related_index,
                                                    UmiLanguageDiagnosticRelated *out_related);
    /* The borrowed JSON span is length-delimited, not necessarily NUL-terminated.
 * Keep it intact if a future reviewed code-action request includes diagnostics. */
    UmiStatus UmiLanguageDiagnosticCatalogueItemJson(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                     size_t index, const char **out_json, size_t *out_bytes);
    /* Match the exact URI and any supplied version, then resolve every primary
 * range and each same-document related range against UTF-8 source. Reject
 * split UTF-16 surrogate pairs, invalid line columns and out-of-draft ranges.
 * Other related URIs remain informational. An absent version is not invented;
 * a host must establish freshness separately, for example with an exclusively
 * owned, newly opened source session. A supplied version needs known_version.
 * Source is limited to 16 MiB by the shared coordinate index. One owned index
 * serves all endpoints. No input or editor state is changed. */
    UmiStatus UmiLanguageDiagnosticCatalogueValidateSource(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                           const char *document_uri,
                                                           const int32_t *known_version, const char *source,
                                                           size_t bytes, const UmiCancellationToken *cancel);
    /* Select a diagnostic's complete source location for explicit navigation.
 * Location zero is the primary reported range; one through related_count are
 * related-information locations in server order. The returned URI is borrowed
 * until catalogue destruction. Invalid input or an out-of-range selection leaves
 * output unchanged. This does not read files or establish external-source
 * freshness. Recheck the captured primary source, copy the URI before callbacks,
 * then ask the document owner to validate and open the selected current range. */
    UmiStatus UmiLanguageDiagnosticCatalogueLocation(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                     size_t diagnostic_index, size_t location_index,
                                                     UmiLanguageSourceLocation *out_location);
#ifdef __cplusplus
}
#endif
#endif
