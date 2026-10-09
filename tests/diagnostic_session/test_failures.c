/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/diagnostic_session/test_failures.c
 * PURPOSE: Exercise partial-write failures, invalid publications and cancellation without replaying requests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    DiagnosticFixture *fixture = calloc(1U, sizeof *fixture);
    CHECK(fixture != NULL);
    DiagnosticCaps(fixture, "{\"textDocumentSync\":2}");
    if (strcmp(argv[1], "fragmented") == 0)
        fixture->fragment = 1U;
    UmiLanguageRuntimeServer *server = DiagnosticServer(fixture);
    UmiLanguageDiagnosticRequest request = DiagnosticRequest();
    UmiLanguageDiagnosticSession *session = NULL;
    CHECK(UmiLanguageDiagnosticSessionStartOnServer(server, &request, NULL, &session) ==
          UMI_STATUS_OK);
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    UmiStatus status;
    if (strcmp(argv[1], "write") == 0)
    {
        fixture->fail_write = 1;
        int32_t version = 77;
        status = UmiLanguageDiagnosticSessionUpdate(session, "changed", 7U, 1, &version);
        CHECK(status == UMI_STATUS_IO_ERROR && version == 77);
        fixture->fail_write = 0;
    }
    else
    {
        UmiStatus expected = UMI_STATUS_OK;
        const char *items = "[]";
        if (strcmp(argv[1], "range") == 0)
        {
            items = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                    "\"character\":4}},\"message\":\"outside\"}]";
            expected = UMI_STATUS_INVALID_ARGUMENT;
        }
        else if (strcmp(argv[1], "malformed") == 0)
        {
            items = "[{\"message\":\"missing range\"}]";
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(argv[1], "read") == 0)
        {
            fixture->fail_read = 1;
            expected = UMI_STATUS_IO_ERROR;
        }
        else
            CHECK(strcmp(argv[1], "fragmented") == 0 || strcmp(argv[1], "unversioned") == 0 ||
                  strcmp(argv[1], "quiet") == 0 || strcmp(argv[1], "cancel") == 0);
        if (strcmp(argv[1], "quiet") != 0)
            DiagnosticPublication(fixture, request.document_uri,
                                  strcmp(argv[1], "unversioned") == 0 ? "" : "\"version\":1,",
                                  items);
        else
            expected = UMI_STATUS_NOT_FOUND;
        UmiCancellationToken *cancel = NULL;
        if (strcmp(argv[1], "cancel") == 0)
        {
            CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
            umi_cancellation_token_request(cancel);
            expected = UMI_STATUS_CANCELLED;
        }
        status = UmiLanguageDiagnosticSessionPoll(session, 100U, cancel, &catalogue);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK)
            CHECK(catalogue != NULL);
        else
            CHECK(catalogue == NULL);
        umi_cancellation_token_destroy(cancel);
        fixture->fail_read = 0;
    }
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    if (status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_CANCELLED)
        DiagnosticFinish(fixture, session, server);
    else
    {
        UmiLanguageDiagnosticSessionSnapshot snapshot;
        CHECK(UmiLanguageDiagnosticSessionRead(session, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.closed && !snapshot.ready && !fixture->running);
        CHECK(UmiLanguageDiagnosticSessionPoll(session, 0U, NULL, &catalogue) ==
              UMI_STATUS_INVALID_STATE);
        UmiLanguageDiagnosticSessionDestroy(session);
        umi_language_runtime_server_destroy(server);
    }
    free(fixture);
    return 0;
}
