/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/diagnostic_session.h
 * PURPOSE: Own an explicitly started language server while one source document changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_SESSION_H
#define UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_SESSION_H
#include "umicom/language_runtime/diagnostic_query.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageDiagnosticSession UmiLanguageDiagnosticSession;
    typedef struct UmiLanguageDiagnosticSessionSnapshot
    {
        int32_t document_version;
        int ready, document_opened, closed;
        uint64_t publications, ignored_publications, unversioned_publications;
        UmiStatus status, close_status, shutdown_status;
    } UmiLanguageDiagnosticSessionSnapshot;
    /** Start a selected native server for one document, copying its URI, language and
 * UTF-8 source. The caller must authorize the executable and project first.
 * All session operations belong to one persistent worker thread. This object is
 * not thread safe, does not watch files, and never edits or saves an editor.
 * The server may read files and start other processes; it is not sandboxed.
 *
 * The request timeout covers initialization reply waits. Source and each wire
 * envelope must fit 65536 bytes, including escaping and protocol fields. Only
 * UTF-16 positions with open/close and full or incremental changes are supported.
 * NULL/empty tool_directory uses the normal launcher selection. Launches and
 * writes are synchronous; cancellation is checked between reads.
 * Failure clears output and closes any child started by this call. */
    UmiStatus UmiLanguageDiagnosticSessionStartNative(const UmiLanguageServerProfile *profile,
                                                      const char *working_directory,
                                                      const char *tool_directory,
                                                      const UmiLanguageDiagnosticRequest *request,
                                                      const UmiCancellationToken *cancel,
                                                      UmiLanguageDiagnosticSession **out);
    /** Borrow an exclusive STARTING server for deterministic transports or a host's
 * selected connection. Preflight failure leaves it untouched. Once initialization
 * starts, failure stops the transport. On success Close stops it, but the caller
 * still destroys the server object after destroying the session. Do not attach
 * this server to a manager or another protocol pump. */
    UmiStatus UmiLanguageDiagnosticSessionStartOnServer(UmiLanguageRuntimeServer *server,
                                                        const UmiLanguageDiagnosticRequest *request,
                                                        const UmiCancellationToken *cancel,
                                                        UmiLanguageDiagnosticSession **out);
    /** Send a complete copied draft after checking the currently accepted version.
 * Identical text is a no-op. Full-sync servers receive full content; incremental
 * servers receive one replacement range spanning the preceding source. All text,
 * coordinates and envelope sizes are checked before writing. Validation failure
 * preserves the current session; a transport failure makes it unusable and stops
 * it rather than retrying a possibly partial notification. Success updates the
 * caller's version; failure leaves it unchanged. No trust or save is implied. */
    UmiStatus UmiLanguageDiagnosticSessionUpdate(UmiLanguageDiagnosticSession *session,
                                                 const char *source, size_t bytes,
                                                 int32_t expected_version, int32_t *out_version);
    /** Wait for one complete diagnostic replacement set matching the current source.
 * Only a matching explicit version is accepted after the first changed draft.
 * An unversioned first publication is accepted because the session has opened
 * only one source revision. Unversioned later reports are counted and ignored.
 * Stale/other-document reports are ignored within the same total wait budget.
 * Empty diagnostics are a valid publication, not proof that indexing is finished.
 *
 * NOT_FOUND means no acceptable publication arrived within timeout_ms (zero polls
 * once). Cancellation leaves the session owned by the caller, which must close it.
 * Protocol, range and transport failures stop the session. Server requests are
 * unsupported and terminate it; no command, link or workspace edit is executed.
 * Success transfers a catalogue; failure clears it. After another Update, discard
 * a previously returned catalogue even if a server has not published again.
 */
    UmiStatus UmiLanguageDiagnosticSessionPoll(UmiLanguageDiagnosticSession *session,
                                               uint32_t timeout_ms,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageDiagnosticCatalogue **out);
    /** Copy state on the owning worker. Counters saturate and do not wrap. */
    UmiStatus UmiLanguageDiagnosticSessionRead(const UmiLanguageDiagnosticSession *session,
                                               UmiLanguageDiagnosticSessionSnapshot *out);
    /** Send didClose and request shutdown, then stop the direct child regardless of
 * protocol errors. A separate 250 ms read budget is used for shutdown. Repeated
 * calls return the recorded cleanup result. Native transport ownership limits
 * remain those of the existing language runtime. */
    UmiStatus UmiLanguageDiagnosticSessionClose(UmiLanguageDiagnosticSession *session);
    /** Close if necessary, release copied source and destroy an owned native server.
 * For a borrowed server, the host destroys it after this returns. */
    void UmiLanguageDiagnosticSessionDestroy(UmiLanguageDiagnosticSession *session);
#ifdef __cplusplus
}
#endif
#endif
