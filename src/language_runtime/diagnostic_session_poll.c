/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_session_poll.c
 * PURPOSE: Accept complete diagnostic sets only for the source revision owned by this session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "diagnostic_session_internal.h"
#include <stdlib.h>
#include <string.h>
static void DiagnosticSessionCount(uint64_t *value)
{
    if (*value != UINT64_MAX)
        ++*value;
}
static UmiStatus DiagnosticSessionPublication(UmiLanguageDiagnosticSession *session,
                                              const UmiLanguageRuntimeEnvelope *reply,
                                              const UmiCancellationToken *cancel,
                                              UmiLanguageDiagnosticCatalogue **out)
{
    if (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE ||
        reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR)
        return UMI_STATUS_PARSE_ERROR; /* No client request is pending during publication polling. */
    if (reply->kind != UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION ||
        strcmp(reply->method, "textDocument/publishDiagnostics") != 0)
        return UMI_STATUS_NOT_FOUND;
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    UmiStatus status = UmiLanguageDiagnosticCatalogueReadNotification(
        reply->json, strlen(reply->json), cancel, &catalogue);
    UmiLanguageDiagnosticPublication publication;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticCataloguePublication(catalogue, &publication);
    if (status == UMI_STATUS_OK)
    {
        bool same_uri = strcmp(publication.uri, session->uri) == 0;
        if (same_uri && !publication.has_version)
            DiagnosticSessionCount(&session->snapshot.unversioned_publications);
        if (!same_uri ||
            (publication.has_version &&
             publication.version != session->snapshot.document_version) ||
            (!publication.has_version && session->snapshot.document_version != 1))
        {
            DiagnosticSessionCount(&session->snapshot.ignored_publications);
            status = UMI_STATUS_NOT_FOUND;
        }
    }
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticCatalogueValidateSource(
            catalogue, session->uri, &session->snapshot.document_version, session->source,
            session->bytes, cancel);
    if (status == UMI_STATUS_OK)
    {
        DiagnosticSessionCount(&session->snapshot.publications);
        *out = catalogue;
    }
    else
        UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    return status;
}
UmiStatus UmiLanguageDiagnosticSessionPoll(UmiLanguageDiagnosticSession *session,
                                           uint32_t timeout_ms, const UmiCancellationToken *cancel,
                                           UmiLanguageDiagnosticCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (session == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!session->snapshot.ready || session->snapshot.closed)
        return UMI_STATUS_INVALID_STATE;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof *reply);
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock);
    UmiStatus status = UMI_STATUS_NOT_FOUND;
    unsigned messages = 0U;
    for (;;)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        uint64_t elapsed = (clock.monotonic_nanoseconds(&clock) - started) / 1000000U;
        uint32_t remaining = elapsed >= timeout_ms ? 0U : timeout_ms - (uint32_t)elapsed;
        status = umi_language_runtime_server_receive(session->server,
                                                     remaining > 50U ? 50U : remaining, reply);
        if (status == UMI_STATUS_OK)
        {
            ++messages;
            status = DiagnosticSessionPublication(session, reply, cancel, out);
        }
        if (status != UMI_STATUS_NOT_FOUND)
            break;
        if (!umi_language_runtime_server_is_running(session->server))
        {
            status = UMI_STATUS_UNAVAILABLE;
            break;
        }
        if (messages >= 4096U)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if ((clock.monotonic_nanoseconds(&clock) - started) / 1000000U >= timeout_ms)
            break;
        (void)clock.sleep_milliseconds(&clock, 1U);
    }
    free(reply);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND && status != UMI_STATUS_CANCELLED)
        return DiagnosticSessionFail(session, status);
    return status;
}
