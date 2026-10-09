/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_session.c
 * PURPOSE: Keep session ownership and copied document revisions independent of application widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "diagnostic_session_internal.h"
#include "server_profile_internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus DiagnosticSessionFail(UmiLanguageDiagnosticSession *session, UmiStatus status)
{
    session->snapshot.status = status;
    session->snapshot.ready = 0;
    session->snapshot.closed = 1;
    session->snapshot.shutdown_status = umi_language_runtime_server_stop(session->server, 0U);
    return status;
}
UmiStatus UmiLanguageDiagnosticSessionStartOnServer(UmiLanguageRuntimeServer *server,
                                                    const UmiLanguageDiagnosticRequest *request,
                                                    const UmiCancellationToken *cancel,
                                                    UmiLanguageDiagnosticSession **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (server == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiLanguageDiagnosticSession *session = NULL;
    DiagnosticSessionWire *wire = NULL;
    UmiStatus status = DiagnosticSessionPrepare(request, &session, &wire);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageRuntimeServerSnapshot snapshot;
    status = umi_language_runtime_server_snapshot(server, &snapshot);
    if (status == UMI_STATUS_OK && (snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_STARTING ||
                                    strcmp(snapshot.root_uri, session->root) != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
    {
        session->server = server;
        status = DiagnosticSessionInitialize(session, wire, request->timeout_ms, cancel);
    }
    free(wire);
    if (status != UMI_STATUS_OK)
    {
        free(session->source);
        free(session);
        return status;
    }
    *out = session;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticSessionStartNative(const UmiLanguageServerProfile *profile,
                                                  const char *working_directory,
                                                  const char *tool_directory,
                                                  const UmiLanguageDiagnosticRequest *request,
                                                  const UmiCancellationToken *cancel,
                                                  UmiLanguageDiagnosticSession **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (LanguageProfileText(profile) != UMI_STATUS_OK || working_directory == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!profile->enabled)
        return UMI_STATUS_UNAVAILABLE;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    /* Preflight source and wire before starting a child. The second API borrows
     * the same checked request, but ownership of a new native child stays here. */
    UmiLanguageDiagnosticSession *prepared = NULL;
    DiagnosticSessionWire *wire = NULL;
    UmiStatus status = DiagnosticSessionPrepare(request, &prepared, &wire);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageRuntimeServer *server = NULL;
    status = UmiLanguageRuntimeServerStartWithToolDirectory("source-diagnostics", profile,
                                                            request->root_uri, working_directory,
                                                            tool_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        prepared->server = server;
        prepared->owns_server = 1;
        status = DiagnosticSessionInitialize(prepared, wire, request->timeout_ms, cancel);
    }
    free(wire);
    if (status != UMI_STATUS_OK)
    {
        umi_language_runtime_server_destroy(server);
        free(prepared->source);
        free(prepared);
        return status;
    }
    *out = prepared;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticSessionUpdate(UmiLanguageDiagnosticSession *session,
                                             const char *source, size_t bytes,
                                             int32_t expected_version, int32_t *out_version)
{
    if (session == NULL || source == NULL || out_version == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!session->snapshot.ready || session->snapshot.closed ||
        expected_version != session->snapshot.document_version)
        return UMI_STATUS_INVALID_STATE;
    UmiEditorTextPosition end;
    UmiStatus status = DiagnosticSessionSource(source, bytes, &end);
    if (status != UMI_STATUS_OK)
        return status;
    if (bytes == session->bytes && memcmp(source, session->source, bytes) == 0)
    {
        *out_version = expected_version;
        return UMI_STATUS_OK;
    }
    if (expected_version == INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *copy = malloc(bytes + 1U), *wire = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (copy == NULL || wire == NULL)
    {
        free(copy);
        free(wire);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, source, bytes);
    copy[bytes] = '\0';
    status = DiagnosticSessionDocument(session, copy, bytes, expected_version + 1, 0, wire);
    if (status == UMI_STATUS_OK)
    {
        status = umi_language_runtime_server_send_notification(session->server,
                                                               "textDocument/didChange", wire);
        if (status != UMI_STATUS_OK)
            status = DiagnosticSessionFail(session, status);
        else
        {
            free(session->source);
            session->source = copy;
            copy = NULL;
            session->bytes = bytes;
            session->snapshot.document_version = expected_version + 1;
            *out_version = session->snapshot.document_version;
        }
    }
    free(wire);
    free(copy);
    return status;
}
UmiStatus UmiLanguageDiagnosticSessionRead(const UmiLanguageDiagnosticSession *session,
                                           UmiLanguageDiagnosticSessionSnapshot *out)
{
    if (session == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = session->snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticSessionClose(UmiLanguageDiagnosticSession *session)
{
    if (session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!session->snapshot.closed)
    {
        if (session->snapshot.document_opened)
        {
            char *wire = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
            if (wire == NULL)
                session->snapshot.close_status = UMI_STATUS_OUT_OF_MEMORY;
            else
            {
                UmiLanguageRuntimeJsonWriter writer;
                umi_language_runtime_json_writer_init(&writer, wire,
                                                      UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
                umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
                umi_language_runtime_json_writer_string(&writer, session->uri);
                umi_language_runtime_json_writer_raw(&writer, "}}");
                session->snapshot.close_status = writer.status;
                if (writer.status == UMI_STATUS_OK)
                    session->snapshot.close_status = umi_language_runtime_server_send_notification(
                        session->server, "textDocument/didClose", wire);
                free(wire);
            }
        }
        session->snapshot.shutdown_status = UmiLanguageRuntimeServerShutdown(session->server, 250U);
        session->snapshot.ready = 0;
        session->snapshot.closed = 1;
    }
    return session->snapshot.close_status != UMI_STATUS_OK ? session->snapshot.close_status
                                                           : session->snapshot.shutdown_status;
}
void UmiLanguageDiagnosticSessionDestroy(UmiLanguageDiagnosticSession *session)
{
    if (session == NULL)
        return;
    (void)UmiLanguageDiagnosticSessionClose(session);
    if (session->owns_server)
        umi_language_runtime_server_destroy(session->server);
    free(session->source);
    free(session);
}
