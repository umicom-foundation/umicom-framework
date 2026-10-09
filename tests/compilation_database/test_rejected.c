/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compilation_database/test_rejected.c
 * PURPOSE: Reject malformed or incomplete command databases without publishing partial rows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
typedef struct RejectedCase
{
    const char *name;
    const char *text;
} RejectedCase;
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const RejectedCase cases[] = {
        {"object", "{}"},
        {"scalar-row", "[1]"},
        {"missing-directory", "[{\"file\":\"main.c\",\"command\":\"cc main.c\"}]"},
        {"relative-directory",
         "[{\"directory\":\"build\",\"file\":\"main.c\",\"command\":\"cc\"}]"},
        {"missing-file", "[{\"directory\":\"" DATABASE_DIRECTORY "\",\"command\":\"cc\"}]"},
        {"no-command", "[{\"directory\":\"" DATABASE_DIRECTORY "\",\"file\":\"main.c\"}]"},
        {"empty-arguments",
         "[{\"directory\":\"" DATABASE_DIRECTORY "\",\"file\":\"main.c\",\"arguments\":[]}]"},
        {"empty-compiler",
         "[{\"directory\":\"" DATABASE_DIRECTORY "\",\"file\":\"main.c\",\"arguments\":[\"\"]}]"},
        {"numeric-argument", "[{\"directory\":\"" DATABASE_DIRECTORY
                             "\",\"file\":\"main.c\",\"arguments\":[\"cc\",7]}]"},
        {"empty-command",
         "[{\"directory\":\"" DATABASE_DIRECTORY "\",\"file\":\"main.c\",\"command\":\"\"}]"},
        {"duplicate-field", "[{\"directory\":\"" DATABASE_DIRECTORY
                            "\",\"file\":\"a.c\",\"f\\u0069le\":\"b.c\",\"command\":\"cc\"}]"},
        {"invalid-output", "[{\"directory\":\"" DATABASE_DIRECTORY
                           "\",\"file\":\"main.c\",\"output\":null,\"command\":\"cc\"}]"},
        {"escaped-nul",
         "[{\"directory\":\"" DATABASE_DIRECTORY "\",\"file\":\"a\\u0000.c\",\"command\":\"cc\"}]"},
        {"partial", "[" DATABASE_ROW ",{\"file\":\"broken.c\"}]"},
        {"syntax", "[" DATABASE_ROW ",]"},
        {"invalid-utf8", "[{\"directory\":\"" DATABASE_DIRECTORY
                         "\",\"file\":\"\xc0\xaf.c\",\"command\":\"cc\"}]"}};
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    {
        if (strcmp(argv[1], cases[i].name) != 0)
            continue;
        UmiCompilationDatabase *database = (UmiCompilationDatabase *)(void *)&i;
        CHECK(ParseDatabase(cases[i].text, &database) != UMI_STATUS_OK);
        CHECK(database == NULL);
        return EXIT_SUCCESS;
    }
    if (strcmp(argv[1], "cancelled") == 0)
    {
        UmiCompilationDatabase *database = NULL;
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        const char text[] = "[" DATABASE_ROW "]";
        CHECK(UmiCompilationDatabaseCreate(text, strlen(text), cancel, &database) ==
              UMI_STATUS_CANCELLED);
        CHECK(database == NULL);
        umi_cancellation_token_reset(cancel);
        CHECK(UmiCompilationDatabaseCreate(text, strlen(text), cancel, &database) == UMI_STATUS_OK);
        UmiCompilationDatabaseDestroy(database);
        umi_cancellation_token_destroy(cancel);
        return EXIT_SUCCESS;
    }
    return 2;
}
