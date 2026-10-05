/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/symbol_query.h
 * PURPOSE: Request an owned document-symbol outline through a temporary native language connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SYMBOL_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_SYMBOL_QUERY_H
#include "umicom/language_runtime/symbol_catalogue.h"
#include "umicom/language_runtime/server_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageSymbolRequest
    {
        const char *root_uri, *document_uri, *language_id, *source;
        size_t source_bytes;
        uint32_t timeout_ms;
    } UmiLanguageSymbolRequest;
    typedef struct UmiLanguageSymbolReport
    {
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t symbols;
    } UmiLanguageSymbolReport;
    /* Request the whole captured document, independent of its caret. Advertise
 * hierarchical symbol support, known kinds and deprecation tags; validate
 * returned ranges against this source whenever their URI matches.
 * No destination is opened and no text is edited. Keep/copy inputs until the
 * call finishes. Failure clears out_catalogue; publication follows cleanup.
 *
 * Use the common 65536-byte native message and parser limits. timeout_ms is
 * one elapsed read budget for initialize and query; cleanup has 250 ms.
 * Cancellation is checked between reads, not during process launch or writes.
 * Own the direct child only. A selected server can inspect the project folder
 * and launch descendants. This is an explicit temporary query, not an index. */
    UmiStatus UmiLanguageSymbolQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageSymbolRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageSymbolReport *out_report,
                                           UmiLanguageSymbolCatalogue **out_catalogue);
    /* Use an exclusively owned STARTING server. Preflight failure leaves it alone;
 * initialization transfers cleanup responsibility to this call. The caller
 * still destroys its server object afterward. */
    UmiStatus UmiLanguageSymbolQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageSymbolRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageSymbolReport *out_report,
                                             UmiLanguageSymbolCatalogue **out_catalogue);
    /* Project search keeps a captured primary draft synchronized while the server
 * searches its workspace. The query is UTF-8, bounded to 4096 bytes; an empty
 * query requests all symbols that the server chooses to return. Other unsaved
 * drafts are not synchronized by this operation. */
    typedef struct UmiLanguageWorkspaceSymbolRequest
    {
        UmiLanguageSymbolRequest source;
        const char *query;
    } UmiLanguageWorkspaceSymbolRequest;
    /* Require workspaceSymbolProvider and complete flat symbol locations. Do not
 * advertise lazy location resolution, infer hierarchy from container labels,
 * open destinations or modify documents. Primary-source ranges are checked
 * before return; other destinations must be checked by the document owner
 * when explicitly opened. Existing query lifetime, timeout, capacity and
 * cleanup rules apply. A temporary connection is not a persistent index. */
    UmiStatus UmiLanguageWorkspaceSymbolQueryNative(const UmiLanguageServerProfile *profile,
                                                    const char *working_directory,
                                                    const UmiLanguageWorkspaceSymbolRequest *request,
                                                    const UmiCancellationToken *cancel,
                                                    UmiLanguageSymbolReport *out_report,
                                                    UmiLanguageSymbolCatalogue **out_catalogue);
    UmiStatus UmiLanguageWorkspaceSymbolQueryOnServer(UmiLanguageRuntimeServer *server,
                                                      const UmiLanguageWorkspaceSymbolRequest *request,
                                                      const UmiCancellationToken *cancel,
                                                      UmiLanguageSymbolReport *out_report,
                                                      UmiLanguageSymbolCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
