/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_session_protocol.c
 * PURPOSE: Negotiate only supported synchronization and retain one initialization deadline.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "diagnostic_session_internal.h"
#include "umicom/language_runtime/response_tree.h"
#include <stdlib.h>
#include <string.h>

/* Missing optional fields use defaults, but duplicate fields remain errors.
 * A server must not select contradictory synchronization settings. */
static UmiStatus DiagnosticSessionMember(const UmiJsonTree *tree, int object, const char *name,
                                         int *out)
{
    *out = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK : status;
}
static UmiStatus DiagnosticSessionCapabilities(const char *json, uint64_t request, int *out_sync)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities = -1, encoding = -1, sync = -1;
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request, &limits, NULL, &tree, &result);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK)
        status = DiagnosticSessionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        char selected[32];
        status = UmiJsonTreeText(tree, encoding, selected, sizeof selected);
        if (status == UMI_STATUS_OK && strcmp(selected, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    /* A legacy clangd override must not silently change position units when
     * the standardized selection was absent or contradictory. */
    if (status == UMI_STATUS_OK)
        status = DiagnosticSessionMember(tree, result, "offsetEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        char selected[32];
        status = UmiJsonTreeText(tree, encoding, selected, sizeof selected);
        if (status == UMI_STATUS_OK && strcmp(selected, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = DiagnosticSessionMember(tree, capabilities, "textDocumentSync", &sync);
    int64_t kind = 0;
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int member = -1, enabled = 0;
        status = DiagnosticSessionMember(tree, sync, "openClose", &member);
        if (status == UMI_STATUS_OK && member < 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeBoolean(tree, member, &enabled);
        if (status == UMI_STATUS_OK && !enabled)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        if (status == UMI_STATUS_OK)
            status = DiagnosticSessionMember(tree, sync, "change", &member);
        if (status == UMI_STATUS_OK && member < 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeInteger(tree, member, &kind);
    }
    else if (status == UMI_STATUS_OK)
    {
        status = sync < 0 ? UMI_STATUS_NOT_IMPLEMENTED : UmiJsonTreeInteger(tree, sync, &kind);
    }
    if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        *out_sync = (int)kind;
    UmiJsonTreeDestroy(tree);
    return status;
}
UmiStatus DiagnosticSessionInitialize(UmiLanguageDiagnosticSession *session,
                                      const DiagnosticSessionWire *wire, uint32_t timeout,
                                      const UmiCancellationToken *cancel)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof *reply);
    if (reply == NULL)
        return DiagnosticSessionFail(session, UMI_STATUS_OUT_OF_MEMORY);
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), request = 0U;
    UmiStatus status = umi_language_runtime_server_transition(
        session->server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(session->server, "initialize",
                                                          wire->initialize, NULL, &request);
    unsigned messages = 0U;
    int received = 0;
    while (status == UMI_STATUS_OK && !received)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        uint64_t elapsed = (clock.monotonic_nanoseconds(&clock) - started) / 1000000U;
        if (elapsed >= timeout && messages != 0U)
        {
            status = UMI_STATUS_TIMEOUT;
            break;
        }
        uint32_t remaining = elapsed >= timeout ? 0U : timeout - (uint32_t)elapsed;
        UmiStatus read = umi_language_runtime_server_receive(
            session->server, remaining > 50U ? 50U : remaining, reply);
        if (read == UMI_STATUS_OK)
        {
            ++messages;
            if (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            else if (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE ||
                     reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR)
            {
                if (reply->request_id != request)
                    status = UMI_STATUS_PARSE_ERROR;
                else
                {
                    status =
                        DiagnosticSessionCapabilities(reply->json, request, &session->sync_kind);
                    received = status == UMI_STATUS_OK;
                }
            }
            if (messages >= 4096U && !received && status == UMI_STATUS_OK)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else if (read != UMI_STATUS_NOT_FOUND)
            status = read;
        if (status == UMI_STATUS_OK && !received)
        {
            if (!umi_language_runtime_server_is_running(session->server))
                status = UMI_STATUS_UNAVAILABLE;
            else if ((clock.monotonic_nanoseconds(&clock) - started) / 1000000U >= timeout)
                status = UMI_STATUS_TIMEOUT;
            else if (read == UMI_STATUS_NOT_FOUND)
                (void)clock.sleep_milliseconds(&clock, 1U);
        }
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status =
            umi_language_runtime_server_send_notification(session->server, "initialized", "{}");
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(session->server,
                                                        UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_notification(
            session->server, "textDocument/didOpen", wire->document);
    if (status == UMI_STATUS_OK)
    {
        session->snapshot.ready = 1;
        session->snapshot.document_opened = 1;
        session->snapshot.document_version = 1;
    }
    else
        status = DiagnosticSessionFail(session, status);
    free(reply);
    return status;
}
