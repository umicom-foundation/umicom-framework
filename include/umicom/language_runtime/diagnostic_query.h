/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/diagnostic_query.h
 * PURPOSE: Capture a source diagnostic publication through an exclusively owned language-server session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_QUERY_H
#include "umicom/language_runtime/diagnostic_catalogue.h"
#include "umicom/language_runtime/server_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageDiagnosticRequest
    {
        const char *root_uri, *document_uri, *language_id, *source;
        size_t source_bytes;
        uint32_t timeout_ms;
    } UmiLanguageDiagnosticRequest;
    typedef struct UmiLanguageDiagnosticReport
    {
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t diagnostics;
    } UmiLanguageDiagnosticReport;
    /* Open one complete captured draft at document version 1 and wait for the first
 * matching publishDiagnostics notification. Ignore other document URIs and
 * explicitly mismatched versions within the same total read budget. An absent
 * version is accepted only because this operation exclusively owns a fresh
 * session and sends no other source revisions. Resolve all local ranges before
 * returning the owned catalogue. An empty publication is an observed empty set,
 * not evidence that background indexing or all project analysis has finished.
 *
 * No compiler command, edit, Save, code action or hyperlink is run. Other
 * unsaved documents are not synchronized. Servers using pull-only diagnostics
 * may time out; there is no persistent diagnostic subscription in this API.
 * Inputs stay alive until return. Failure clears output. Native messages are
 * bounded to 65536 bytes including escaped source/envelope; native token limits
 * may be lower than catalogue limits. Initialization and publication share one
 * timeout, followed by at most 250 ms of read cleanup. Cancellation occurs
 * between reads, not during launch/writes; only the direct child is owned.
 * A selected server can read project files and launch descendants. Publish
 * results only after didClose and shutdown succeed. */
    UmiStatus UmiLanguageDiagnosticQueryNative(const UmiLanguageServerProfile *profile,
                                               const char *working_directory,
                                               const UmiLanguageDiagnosticRequest *request,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageDiagnosticReport *out_report,
                                               UmiLanguageDiagnosticCatalogue **out_catalogue);
    /* STARTING server, exclusively owned by this request. Invalid preflight does
 * not touch it. Once initialization begins the query owns cleanup; the caller
 * destroys the server object after return, including on failure. */
    UmiStatus UmiLanguageDiagnosticQueryOnServer(UmiLanguageRuntimeServer *server,
                                                 const UmiLanguageDiagnosticRequest *request,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageDiagnosticReport *out_report,
                                                 UmiLanguageDiagnosticCatalogue **out_catalogue);
    /* Explicit pull mode negotiates diagnosticProvider and requests a full report
 * with textDocument/diagnostic after opening the captured draft. A declared
 * provider identifier is sent unchanged. No previous result ID is supplied;
 * unchanged responses and nonempty related-document reports are refused.
 * The same captured-source checks, transport bounds, timeout, cancellation
 * and cleanup ownership as the notification APIs above apply. This operation
 * does not fall back to notifications or establish a persistent subscription. */
    UmiStatus UmiLanguageDiagnosticQueryPullNative(const UmiLanguageServerProfile *profile,
                                                   const char *working_directory,
                                                   const UmiLanguageDiagnosticRequest *request,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageDiagnosticReport *out_report,
                                                   UmiLanguageDiagnosticCatalogue **out_catalogue);
    UmiStatus UmiLanguageDiagnosticQueryPullOnServer(UmiLanguageRuntimeServer *server,
                                                     const UmiLanguageDiagnosticRequest *request,
                                                     const UmiCancellationToken *cancel,
                                                     UmiLanguageDiagnosticReport *out_report,
                                                     UmiLanguageDiagnosticCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
