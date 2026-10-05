/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/diagnostic_related_fixture.inc
 * PURPOSE: Construct independently authored related-location publications for native navigation tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* The primary URI is already a quoted JSON token copied by the independent
 * stream peer. Related URIs are fixed fixture literals, never user input. */
static int DiagnosticRelatedFixtureItems(const char *mode, const char *primary, char *output, size_t capacity)
{
    const char *uri = strcmp(mode, "diagnostics-links-remote") == 0      ? "https://example.invalid/source.c"
                      : strcmp(mode, "diagnostics-links-authority") == 0 ? "file://server/share/source.c"
                      : strcmp(mode, "diagnostics-links-query") == 0 ? "file:///workspace/related.c?debug=1"
                                                                     : "file:///workspace/related.c";
    unsigned end = strcmp(mode, "diagnostics-links-unicode") == 0   ? 2U
                   : strcmp(mode, "diagnostics-links-outside") == 0 ? 99U
                                                                    : 1U;
    const char *message =
        strcmp(mode, "diagnostics-links-literal") == 0 ? "<b>declared & used</b>" : "Other declaration";
    char related[4096];
    int n =
        snprintf(related, sizeof(related),
                 ",\"relatedInformation\":[{\"message\":\"Same "
                 "draft\",\"location\":{\"uri\":%s,\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":"
                 "{\"line\":0,\"character\":2}}}},{\"message\":\"%s\",\"location\":{\"uri\":\"%s\",\"range\":"
                 "{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":1,\"character\":%u}}}}]",
                 primary, message, uri, end);
    if (n < 0 || (size_t)n >= sizeof(related))
        return 0;
    const char *additional = strcmp(mode, "diagnostics-links-choices") == 0
                                 ? ",{\"message\":\"Another "
                                   "diagnostic\",\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
                                   "\"line\":0,\"character\":2}}}"
                                 : "";
    n = snprintf(output, capacity,
                 "[{\"message\":\"Unknown "
                 "name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":"
                 "2}}%s}%s]",
                 strcmp(mode, "diagnostics-links-none") == 0 ? "" : related, additional);
    return n > 0 && (size_t)n < capacity;
}
