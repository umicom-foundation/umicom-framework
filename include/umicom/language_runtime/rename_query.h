/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/rename_query.h
 * PURPOSE: Request text-only symbol rename proposals through the shared captured-source connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_RENAME_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_RENAME_QUERY_H
#include "umicom/language_runtime/workspace_edit_catalogue.h"
#include "umicom/language_runtime/server_manager.h"
#include "umicom/language_runtime/query_sources.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageRenameRequest
    {
        const char *root_uri, *document_uri, *language_id, *source;
        size_t source_bytes, caret;
        uint32_t timeout_ms;
        const char *new_name;
    } UmiLanguageRenameRequest;
    typedef struct UmiLanguageRenameReport
    {
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t documents;
    } UmiLanguageRenameReport;
    /* Send one captured draft at protocol version 1 and request a text-only rename.
 * When advertised by the server, prepareRename must return an exact source
 * range containing the caret (the end boundary is also accepted). A null
 * preparation returns NOT_FOUND. Default-behavior preparation is not advertised
 * or accepted. The new name is nonempty UTF-8, at most 1024 bytes, without ASCII
 * control characters; the server determines the language's naming rules.
 *
 * Returned edits for the captured URI are checked against its complete source,
 * including versions and overlap. Other documents remain unresolved: callers
 * must capture and validate every target, preserve annotations and collect
 * confirmations before offering application. No file operation, save, edit or
 * server command is performed here. Other unsaved drafts are not synchronized.
 * Null rename results yield an empty catalogue. Failure clears the output.
 *
 * This temporary connection owns only its direct child process. A selected
 * server can inspect project files or start descendants. Native messages allow
 * 65536 bytes, including the envelope and escaped source; parser limits can
 * be lower than catalogue limits. Keep inputs alive until return. timeout_ms
 * covers all reads, including preparation; cleanup permits another 250 ms.
 * Cancellation is checked between reads, not during process launch or writes.
 * Publish results only after didClose and shutdown both succeed. */
    UmiStatus UmiLanguageRenameQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageRenameRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageRenameReport *out_report,
                                           UmiLanguageWorkspaceEditCatalogue **out_catalogue);
    /* An exclusively owned STARTING server transfers cleanup to this operation
 * after successful preflight. Invalid inputs leave it untouched. Destroy the
 * server object after return, even when the request fails. */
    UmiStatus UmiLanguageRenameQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageRenameRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageRenameReport *out_report,
                                             UmiLanguageWorkspaceEditCatalogue **out_catalogue);

    /* Synchronize the primary draft and additional sources before preparation or
 * rename. At most 64 documents total are accepted. Duplicate exact URIs and
 * malformed/oversized messages fail before starting or taking over a server.
 * Every successfully opened document is closed during cleanup, including after
 * a query failure. Publication requires all close operations and shutdown to
 * succeed. Each message has the same native size limit as a single-source query.
 * Returned targets outside this captured set remain unresolved: the caller must
 * refuse their application until it can capture and validate them as well.
 * All earlier rename ownership, timeout, cancellation and review rules apply. */
    UmiStatus UmiLanguageRenameQueryNativeWithSources(const UmiLanguageServerProfile *profile,
                                                      const char *working_directory,
                                                      const UmiLanguageRenameRequest *request,
                                                      const UmiLanguageQuerySource *additional_sources,
                                                      size_t source_count, const UmiCancellationToken *cancel,
                                                      UmiLanguageRenameReport *out_report,
                                                      UmiLanguageWorkspaceEditCatalogue **out_catalogue);
    UmiStatus UmiLanguageRenameQueryOnServerWithSources(UmiLanguageRuntimeServer *server,
                                                        const UmiLanguageRenameRequest *request,
                                                        const UmiLanguageQuerySource *additional_sources,
                                                        size_t source_count,
                                                        const UmiCancellationToken *cancel,
                                                        UmiLanguageRenameReport *out_report,
                                                        UmiLanguageWorkspaceEditCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
