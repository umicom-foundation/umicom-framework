/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/signature_query.h
 * PURPOSE: Request complete parameter help at a captured source position through a temporary native connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SIGNATURE_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_SIGNATURE_QUERY_H
#include "umicom/language_runtime/signature_catalogue.h"
#include "umicom/language_runtime/server_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageSignatureRequest
    {
        const char *root_uri, *document_uri, *language_id, *source;
        size_t source_bytes, caret;
        uint32_t timeout_ms;
    } UmiLanguageSignatureRequest;
    typedef struct UmiLanguageSignatureReport
    {
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t signatures;
    } UmiLanguageSignatureReport;
    /* Explicit invocation sends the captured draft and its exact UTF-16 caret.
 * Advertise literal documentation, UTF-16 label offsets and per-signature
 * active parameters. No edits, files, URLs or server commands are applied.
 * Keep inputs alive until return; failure clears the catalogue output. Publish
 * only after didClose and shutdown succeed. Uses the shared 65536-byte native
 * message and parser limits, which can be smaller than catalogue limits.
 * timeout_ms is one elapsed read budget; cleanup allows another 250 ms.
 * Cancellation is checked between reads, not during launch or writes. Only
 * the direct child is owned; a chosen server can inspect the project or start
 * descendants. This call does not keep a persistent editor connection. */
    UmiStatus UmiLanguageSignatureQueryNative(const UmiLanguageServerProfile *profile,
                                              const char *working_directory,
                                              const UmiLanguageSignatureRequest *request,
                                              const UmiCancellationToken *cancel,
                                              UmiLanguageSignatureReport *out_report,
                                              UmiLanguageSignatureCatalogue **out_catalogue);
    /* Use an exclusively owned STARTING server. Failed preflight leaves it alone;
 * initialization transfers cleanup to this operation. Destroy the server
 * object after return, including when the query fails. */
    UmiStatus UmiLanguageSignatureQueryOnServer(UmiLanguageRuntimeServer *server,
                                                const UmiLanguageSignatureRequest *request,
                                                const UmiCancellationToken *cancel,
                                                UmiLanguageSignatureReport *out_report,
                                                UmiLanguageSignatureCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
