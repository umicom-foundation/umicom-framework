/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_reuse/test_command_catalogue.c
 *
 * PURPOSE:
 *   Refuse ambiguous test owners before a cached qualification can be reused.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/ctest_commands.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "Check failed at %d: %s\n", __LINE__, #value); return 1; } } while (0)
#define DOCUMENT(tests) "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":" tests "}"
static int Reject(const char *text, UmiStatus expected)
{
    UmiCtestCommandCatalogue *catalogue = NULL;
    UmiStatus status = UmiCtestCommandCatalogueParse(text, strlen(text), &catalogue);
    CHECK(status == expected);
    CHECK(catalogue == NULL);
    return 0;
}
int main(void)
{
    const char *json = DOCUMENT("["
        "{\"name\":\"zeta\",\"command\":[\"C:/work folder/test.exe\",\"--case\",\"one\"]},"
        "{\"name\":\"alpha\",\"command\":[\"/tmp/\\u03bb/test\"]},"
        "{\"name\":\"absent\"},"
        "{\"name\":\"empty\",\"command\":[]}]");
    UmiCtestCommandCatalogue *catalogue = NULL;
    CHECK(UmiCtestCommandCatalogueParse(json, strlen(json), &catalogue) == UMI_STATUS_OK);
    CHECK(UmiCtestCommandCatalogueCount(catalogue) == 4U);
    const char *program = NULL;
    CHECK(UmiCtestCommandCatalogueFind(catalogue, "alpha", &program) == UMI_STATUS_OK);
    CHECK(strcmp(program, "/tmp/\xce\xbb/test") == 0);
    CHECK(UmiCtestCommandCatalogueFind(catalogue, "zeta", &program) == UMI_STATUS_OK);
    CHECK(strcmp(program, "C:/work folder/test.exe") == 0);
    const char *sentinel = program;
    CHECK(UmiCtestCommandCatalogueFind(catalogue, "absent", &program) == UMI_STATUS_NOT_FOUND);
    CHECK(program == sentinel);
    CHECK(UmiCtestCommandCatalogueFind(catalogue, "empty", &program) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiCtestCommandCatalogueFind(catalogue, "Alpha", &program) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiCtestCommandCatalogueFind(NULL, "alpha", &program) == UMI_STATUS_INVALID_ARGUMENT);
    UmiCtestCommandCatalogueDestroy(catalogue);
    CHECK(Reject(DOCUMENT("[{\"name\":\"a\"},{\"name\":\"a\"}]"), UMI_STATUS_ALREADY_EXISTS) == 0);
    CHECK(Reject(DOCUMENT("[{\"name\":\"a\",\"name\":\"b\"}]"), UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Reject(DOCUMENT("[{\"name\":\"a\",\"command\":[\"ok\",1]}]"), UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Reject(DOCUMENT("[{\"name\":\"a\",\"command\":\"not an array\"}]"), UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Reject(DOCUMENT("[{\"name\":\"a\",\"command\":[\"\"]}]"), UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Reject(DOCUMENT("[{\"name\":\"a\",\"command\":[\"x\"],\"command\":[\"y\"]}]"), UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Reject(DOCUMENT("[{\"name\":\"\\u0000\"}]"), UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Reject(DOCUMENT("[{\"name\":\"\\ud800\"}]"), UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Reject("{\"tests\":[]}", UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(Reject("{\"kind\":\"ctestInfo\",\"version\":{\"major\":2,\"minor\":0},\"tests\":[]}", UMI_STATUS_NOT_IMPLEMENTED) == 0);
    CHECK(Reject(DOCUMENT("[]") "garbage", UMI_STATUS_PARSE_ERROR) == 0);
    CHECK(UmiCtestCommandCatalogueParse(json, strlen(json) - 1U, &catalogue) == UMI_STATUS_PARSE_ERROR);
    CHECK(catalogue == NULL);
    const char embedded[] = DOCUMENT("[]") "\0more";
    CHECK(UmiCtestCommandCatalogueParse(embedded, sizeof embedded - 1U, &catalogue) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiCtestCommandCatalogueParse(DOCUMENT("[]"), strlen(DOCUMENT("[]")), &catalogue) == UMI_STATUS_OK);
    CHECK(UmiCtestCommandCatalogueCount(catalogue) == 0U);
    UmiCtestCommandCatalogueDestroy(catalogue);
    UmiCtestCommandCatalogueDestroy(NULL);
    return 0;
}
