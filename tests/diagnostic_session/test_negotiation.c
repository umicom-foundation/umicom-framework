/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/diagnostic_session/test_negotiation.c
 * PURPOSE: Check negotiated position units, synchronization capability and failure ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    DiagnosticFixture *fixture = calloc(1U, sizeof *fixture);
    CHECK(fixture != NULL);
    const char *caps = "{\"textDocumentSync\":2}";
    UmiStatus expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(argv[1], "full") == 0)
    {
        caps = "{\"textDocumentSync\":1}";
        expected = UMI_STATUS_OK;
    }
    else if (strcmp(argv[1], "incremental") == 0)
        expected = UMI_STATUS_OK;
    else if (strcmp(argv[1], "object") == 0)
    {
        caps = "{\"textDocumentSync\":{\"openClose\":true,\"change\":2}}";
        expected = UMI_STATUS_OK;
    }
    else if (strcmp(argv[1], "missing-change") == 0)
        caps = "{\"textDocumentSync\":{\"openClose\":true}}";
    else if (strcmp(argv[1], "closed") == 0)
        caps = "{\"textDocumentSync\":{\"openClose\":false,\"change\":2}}";
    else if (strcmp(argv[1], "none") == 0)
        caps = "{\"textDocumentSync\":0}";
    else if (strcmp(argv[1], "encoding") == 0)
        caps = "{\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\"}";
    else if (strcmp(argv[1], "duplicate") == 0)
    {
        caps = "{\"textDocumentSync\":1,\"textDocumentSync\":2}";
        expected = UMI_STATUS_ALREADY_EXISTS;
    }
    else if (strcmp(argv[1], "legacy") == 0)
    {
        DiagnosticPush(fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"offsetEncoding\":"
                                "\"utf-8\",\"capabilities\":{\"textDocumentSync\":2}}}");
    }
    else if (strcmp(argv[1], "request") == 0)
    {
        DiagnosticPush(
            fixture,
            "{\"jsonrpc\":\"2.0\",\"id\":9,\"method\":\"workspace/configuration\",\"params\":{}}");
    }
    else if (strcmp(argv[1], "timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    else if (strcmp(argv[1], "cancel") == 0)
        expected = UMI_STATUS_CANCELLED;
    else
        CHECK(0);
    if (strcmp(argv[1], "legacy") && strcmp(argv[1], "request") && strcmp(argv[1], "timeout"))
        DiagnosticCaps(fixture, caps);
    UmiLanguageRuntimeServer *server = DiagnosticServer(fixture);
    UmiCancellationToken *cancel = NULL;
    if (strcmp(argv[1], "cancel") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
    }
    UmiLanguageDiagnosticRequest request = DiagnosticRequest();
    UmiLanguageDiagnosticSession *session = NULL;
    CHECK(UmiLanguageDiagnosticSessionStartOnServer(server, &request, cancel, &session) ==
          expected);
    if (expected == UMI_STATUS_OK)
    {
        UmiLanguageDiagnosticSessionSnapshot snapshot;
        CHECK(UmiLanguageDiagnosticSessionRead(session, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.ready && snapshot.document_opened && snapshot.document_version == 1);
        CHECK(strstr(fixture->written, "\"versionSupport\":true") != NULL);
        DiagnosticFinish(fixture, session, server);
    }
    else
    {
        CHECK(session == NULL);
        if (expected == UMI_STATUS_CANCELLED)
            CHECK(fixture->stops == 0U && fixture->written_bytes == 0U);
        else
            CHECK(fixture->stops > 0U && !fixture->running);
        umi_language_runtime_server_destroy(server);
    }
    umi_cancellation_token_destroy(cancel);
    free(fixture);
    return 0;
}
