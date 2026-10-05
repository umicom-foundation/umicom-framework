/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/hover_query.h
 * PURPOSE: Request source information at a captured caret through an owned native connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_HOVER_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_HOVER_QUERY_H
#include "umicom/language_runtime/hover_document.h"
#include "umicom/language_runtime/server_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageHoverRequest
    {
        const char *root_uri, *document_uri, *language_id;
        const char *source;
        size_t source_bytes, caret;
        uint32_t timeout_ms;
    } UmiLanguageHoverRequest;
    typedef struct UmiLanguageHoverReport
    {
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t blocks;
    } UmiLanguageHoverReport;
    /* Request source information at an exact UTF-8 caret from a user-selected
 * trusted server. Copy the source and settings before dispatching to a worker.
 * The server receives the captured draft and may inspect the project folder.
 * Advertise UTF-16 positions and prefer plain text; preserve any returned
 * markdown literally. No returned code, HTML or link is executed.
 *
 * Require hover and document open/close support. Native messages, including
 * escaped source and envelope, must fit the 65536-byte transport capacity and
 * its existing token limit. One elapsed read budget covers initialization and
 * the hover reply. Cancellation is checked between reads, not during launch
 * or synchronous writes. Close and stop the direct child before publication,
 * using a separate 250 ms shutdown reply budget. No file or document is edited.
 * Validate an optional returned range against this captured source.
 * Failure clears the owned result. started means initialization was attempted.
 * This is an explicit temporary lookup, not a persistent mouse-hover session. */
    UmiStatus
    UmiLanguageHoverQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                const UmiLanguageHoverRequest *request, const UmiCancellationToken *cancel,
                                UmiLanguageHoverReport *out_report, UmiLanguageHoverDocument **out_document);
    /* Use an exclusive caller-owned STARTING connection. Preflight leaves it
 * untouched; after initialization begins it is stopped but never destroyed.
 * Do not share it with a manager or another protocol pump. */
    UmiStatus UmiLanguageHoverQueryOnServer(UmiLanguageRuntimeServer *server,
                                            const UmiLanguageHoverRequest *request,
                                            const UmiCancellationToken *cancel,
                                            UmiLanguageHoverReport *out_report,
                                            UmiLanguageHoverDocument **out_document);
#ifdef __cplusplus
}
#endif
#endif
