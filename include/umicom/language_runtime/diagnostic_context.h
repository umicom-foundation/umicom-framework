/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/diagnostic_context.h
 * PURPOSE: Own exact diagnostic context for a reviewed source-range code-action request.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_CONTEXT_H
#define UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_CONTEXT_H
#include "umicom/language_runtime/diagnostic_catalogue.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageDiagnosticContext UmiLanguageDiagnosticContext;
    /* Validate the complete publication against the captured draft, then copy the
 * original JSON of diagnostics relevant to an ordered UTF-8 byte selection.
 * Nonempty ranges overlap as half-open intervals. A caret or zero-length
 * diagnostic also matches a touching boundary, so a missing-token diagnostic
 * at the end of a selection remains available. Related rows do not select a
 * primary diagnostic on their own.
 *
 * Preserve original order, code types and opaque data byte-for-byte. No file
 * access, request or edit occurs. A host must keep this context within the
 * server session that published it; source equality alone does not make opaque
 * server data portable between processes. Bounds follow the catalogue and its
 * 16 MiB source index; the selected JSON array is limited to 1 MiB. Failure
 * clears output. The catalogue and source may be released after success. */
    UmiStatus UmiLanguageDiagnosticContextCreate(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                 const char *document_uri, const int32_t *known_version,
                                                 const char *source, size_t source_bytes, size_t range_start,
                                                 size_t range_end, const UmiCancellationToken *cancel,
                                                 UmiLanguageDiagnosticContext **out_context);
    void UmiLanguageDiagnosticContextDestroy(UmiLanguageDiagnosticContext *context);
    size_t UmiLanguageDiagnosticContextCount(const UmiLanguageDiagnosticContext *context);
    /* Borrow a NUL-terminated JSON array and its byte length, excluding NUL.
 * The view lasts until Destroy. On failure, pointer and size are cleared. */
    UmiStatus UmiLanguageDiagnosticContextJson(const UmiLanguageDiagnosticContext *context,
                                               const char **out_json, size_t *out_bytes);
#ifdef __cplusplus
}
#endif
#endif
