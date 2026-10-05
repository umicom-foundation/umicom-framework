/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/completion_query.h
 * PURPOSE: Request native language completions for an explicitly captured source snapshot.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_COMPLETION_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_COMPLETION_QUERY_H
#include "umicom/language_runtime/completion_catalogue.h"
#include "umicom/language_runtime/server_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageCompletionRequest
    {
        const char *root_uri, *document_uri, *language_id;
        const char *source;
        size_t source_bytes, cursor_offset;
        uint32_t timeout_ms;
    } UmiLanguageCompletionRequest;
    typedef struct UmiLanguageCompletionQueryReport
    {
        /* started means protocol initialization was attempted, not just launch. */
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t choices;
    } UmiLanguageCompletionQueryReport;
    /* Explicit, isolated completion query. Start only a user-selected trusted
 * executable, initialize it with plain-text completion capabilities, send a
 * captured didOpen draft and request choices at its UTF-8 caret offset.
 * The server may inspect its working folder. No live document, saved file,
 * credential store or application widget is changed.
 *
 * Call on a worker with copied inputs. Inputs are borrowed until return.
 * Maximum source is 65535 bytes and the escaped JSON must fit the transport's
 * 65536-byte envelope. Replies retain the transport's token/byte limits.
 * Advertise only UTF-16 positions, plain text and supported edit defaults.
 * Refuse an incompatible encoding, absent completion/open-close support or a
 * server-to-client request that this isolated workflow cannot service.
 *
 * One elapsed read budget covers initialization and completion. Cancellation
 * is observed between reads, not during native launch or synchronous writes.
 * Close the document and shut down the direct child before returning, with
 * a separate 250 ms shutdown reply budget. This is not a persistent session.
 * Failure clears out_catalogue. The report separates operation and cleanup
 * failures. Success transfers the owned catalogue to the caller. */
    UmiStatus UmiLanguageCompletionQueryNative(const UmiLanguageServerProfile *profile,
                                               const char *working_directory,
                                               const UmiLanguageCompletionRequest *request,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageCompletionQueryReport *out_report,
                                               UmiLanguageCompletionCatalogue **out_catalogue);
    /* Use an exclusive caller-owned STARTING connection instead of launching.
 * Preflight failure leaves it untouched. Once initialization starts this call
 * always stops the connection before returning, but never destroys the server
 * object. Do not attach it to a manager or share it with another protocol pump.
 * This entry point also permits deterministic transport regression fixtures. */
    UmiStatus UmiLanguageCompletionQueryOnServer(UmiLanguageRuntimeServer *server,
                                                 const UmiLanguageCompletionRequest *request,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageCompletionQueryReport *out_report,
                                                 UmiLanguageCompletionCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
