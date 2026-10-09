/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/diagnostic_session/test_updates.c
 * PURPOSE: Verify exact draft changes, preflight preservation and version-bound publication filtering.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    DiagnosticFixture *fixture = calloc(1U, sizeof *fixture);
    CHECK(fixture != NULL);
    bool full = strcmp(argv[1], "full") == 0;
    DiagnosticCaps(fixture, full ? "{\"textDocumentSync\":1}" : "{\"textDocumentSync\":2}");
    UmiLanguageRuntimeServer *server = DiagnosticServer(fixture);
    UmiLanguageDiagnosticRequest request = DiagnosticRequest();
    request.source = "a\xf0\x9f\x98\x80\r\nz";
    request.source_bytes = strlen(request.source);
    UmiLanguageDiagnosticSession *session = NULL;
    CHECK(UmiLanguageDiagnosticSessionStartOnServer(server, &request, NULL, &session) ==
          UMI_STATUS_OK);
    size_t before = fixture->written_bytes;
    int32_t version = 1;
    if (strcmp(argv[1], "same") == 0)
    {
        CHECK(UmiLanguageDiagnosticSessionUpdate(session, request.source, request.source_bytes, 1,
                                                 &version) == UMI_STATUS_OK);
        CHECK(version == 1 && fixture->written_bytes == before);
    }
    else if (strcmp(argv[1], "stale") == 0)
    {
        CHECK(UmiLanguageDiagnosticSessionUpdate(session, "next", 4U, 0, &version) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(version == 1 && fixture->written_bytes == before);
    }
    else if (strcmp(argv[1], "invalid") == 0 || strcmp(argv[1], "capacity") == 0)
    {
        char *source = NULL;
        if (strcmp(argv[1], "capacity") == 0)
        {
            source = malloc(16001U);
            CHECK(source != NULL);
            memset(source, '\t', 16000U);
            source[16000] = '\0';
            /* Six-byte JSON escapes for control bytes exceed the transport. */
            memset(source, '\1', 16000U);
            CHECK(UmiLanguageDiagnosticSessionUpdate(session, source, 16000U, 1, &version) ==
                  UMI_STATUS_CAPACITY_EXCEEDED);
        }
        else
            CHECK(UmiLanguageDiagnosticSessionUpdate(session, "\xc0\xaf", 2U, 1, &version) !=
                  UMI_STATUS_OK);
        CHECK(version == 1 && fixture->written_bytes == before);
        free(source);
    }
    else
    {
        CHECK(full || strcmp(argv[1], "incremental") == 0 || strcmp(argv[1], "freshness") == 0);
        CHECK(UmiLanguageDiagnosticSessionUpdate(session, "next", 4U, 1, &version) ==
              UMI_STATUS_OK);
        CHECK(version == 2);
        const char *change = strstr(fixture->written + before, "textDocument/didChange");
        CHECK(change != NULL);
        if (full)
            CHECK(strstr(change, "\"range\"") == NULL);
        else
            CHECK(strstr(change, "\"end\":{\"line\":1,\"character\":1}") != NULL);
        DiagnosticPublication(fixture, request.document_uri, "\"version\":1,", "[]");
        DiagnosticPublication(fixture, request.document_uri, "", "[]");
        DiagnosticPublication(fixture, "file:///other.c", "\"version\":2,", "[]");
        DiagnosticPublication(fixture, request.document_uri, "\"version\":2,", "[]");
        UmiLanguageDiagnosticCatalogue *catalogue = NULL;
        CHECK(UmiLanguageDiagnosticSessionPoll(session, 100U, NULL, &catalogue) == UMI_STATUS_OK);
        CHECK(UmiLanguageDiagnosticCatalogueCount(catalogue) == 0U);
        UmiLanguageDiagnosticCatalogueDestroy(catalogue);
        UmiLanguageDiagnosticSessionSnapshot snapshot;
        CHECK(UmiLanguageDiagnosticSessionRead(session, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.ignored_publications == 3U && snapshot.unversioned_publications == 1U &&
              snapshot.publications == 1U);
    }
    DiagnosticFinish(fixture, session, server);
    free(fixture);
    return 0;
}
