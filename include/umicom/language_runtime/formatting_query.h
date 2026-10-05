/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/formatting_query.h
 * PURPOSE: Request an owned formatting preview for a captured source document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_FORMATTING_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_FORMATTING_QUERY_H
#include "umicom/language_runtime/text_edit_preview.h"
#include "umicom/language_runtime/server_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageFormattingRequest
    {
        const char *root_uri, *document_uri, *language_id;
        const char *source;
        size_t source_bytes, caret;
        uint32_t timeout_ms, tab_size;
        int insert_spaces;
    } UmiLanguageFormattingRequest;
    typedef struct UmiLanguageFormattingReport
    {
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t edit_count;
    } UmiLanguageFormattingReport;
    /* Explicit temporary request against a user-selected trusted local server.
 * Copy source/settings before dispatching to a worker. tab_size is 1..32 and
 * insert_spaces is exactly zero or one. Advertise UTF-16 positions, static
 * formatting support, and no workspace mutation or configuration callbacks.
 * Require document formatting and document open/close support from the server.
 * The native transport's 65536-byte message capacity includes the envelope and
 * escaped source, with its existing token limit. Preflight failure starts no
 * process. The selected server can itself inspect the project folder.
 *
 * One elapsed read budget covers initialization and formatting. Cancellation
 * is checked between reads, not during native launch or synchronous writes.
 * Close the captured document and stop the direct child before publication;
 * shutdown has a separate 250 ms reply budget. This is not persistent indexing.
 * Nothing is applied to a live document or saved file. Success transfers the
 * preview; failure clears it. The report distinguishes query and cleanup.
 * started means protocol initialization was attempted, not merely launch. */
    UmiStatus UmiLanguageFormattingQueryNative(const UmiLanguageServerProfile *profile,
                                               const char *working_directory,
                                               const UmiLanguageFormattingRequest *request,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageFormattingReport *out_report,
                                               UmiLanguageTextEditPreview **out_preview);
    /* Use an exclusive STARTING connection. Preflight leaves it untouched; after
 * initialization starts it is stopped before return but remains caller-owned.
 * Do not share it with a manager or another protocol pump. */
    UmiStatus UmiLanguageFormattingQueryOnServer(UmiLanguageRuntimeServer *server,
                                                 const UmiLanguageFormattingRequest *request,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageFormattingReport *out_report,
                                                 UmiLanguageTextEditPreview **out_preview);
    /* A formatting range is a pair of exact UTF-8 byte boundaries in the captured
 * source. Empty ranges are allowed by the SDK; start must not exceed end.
 * The preview caret remains source.caret and is independent of this range. */
    typedef struct UmiLanguageRangeFormattingRequest
    {
        UmiLanguageFormattingRequest source;
        size_t start, end;
    } UmiLanguageRangeFormattingRequest;
    /* Negotiate range formatting explicitly. Returned edits may adjust surrounding
 * source, so the result is a complete draft preview rather than a clipped edit
 * subset. Review that full result before applying through the document owner.
 * Existing limits, source ownership and native cleanup rules apply. */
    UmiStatus UmiLanguageRangeFormattingQueryNative(const UmiLanguageServerProfile *profile,
                                                    const char *working_directory,
                                                    const UmiLanguageRangeFormattingRequest *request,
                                                    const UmiCancellationToken *cancel,
                                                    UmiLanguageFormattingReport *out_report,
                                                    UmiLanguageTextEditPreview **out_preview);
    UmiStatus UmiLanguageRangeFormattingQueryOnServer(UmiLanguageRuntimeServer *server,
                                                      const UmiLanguageRangeFormattingRequest *request,
                                                      const UmiCancellationToken *cancel,
                                                      UmiLanguageFormattingReport *out_report,
                                                      UmiLanguageTextEditPreview **out_preview);
#ifdef __cplusplus
}
#endif
#endif
