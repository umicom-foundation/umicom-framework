/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_sources/test_catalog.c
 * PURPOSE: Check source descriptor bounds, optional metadata and unchanged outputs on malformed catalogues.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/source_catalog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            failed = 1;                                                                            \
            goto done;                                                                             \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *body = NULL;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "normal") == 0)
        body = "{\"sources\":[{\"name\":\"generated.c\",\"sourceReference\":42,\"origin\":"
               "\"compiler\"}]}";
    else if (strcmp(mode, "empty") == 0)
        body = "{\"sources\":[]}";
    else if (strcmp(mode, "optional") == 0)
        body = "{\"sources\":[{}]}";
    else if (strcmp(mode, "zero") == 0)
        body = "{\"sources\":[{\"path\":\"/remote/file.c\",\"sourceReference\":0}]}";
    else if (strcmp(mode, "max-reference") == 0)
        body = "{\"sources\":[{\"sourceReference\":2147483647}]}";
    else if (strcmp(mode, "repeated") == 0)
        body = "{\"sources\":[{\"name\":\"a\",\"sourceReference\":1},{\"name\":\"b\","
               "\"sourceReference\":1}]}";
    else if (strcmp(mode, "unicode") == 0)
        body = "{\"sources\":[{\"name\":\"caf\\u00e9.c\",\"origin\":\"\\uD83D\\uDE00\"}]}";
    else if (strcmp(mode, "related") == 0)
        body = "{\"sources\":[{\"sources\":[{\"name\":\"original.c\"}]}]}";
    else if (strcmp(mode, "escaped-keys") == 0)
        body = "{\"sourc\\u0065s\":[{\"n\\u0061me\":\"a\",\"sourceRef\\u0065rence\":42}]}";
    else if (strcmp(mode, "negative") == 0)
    {
        body = "{\"sources\":[{\"sourceReference\":-1}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "large-reference") == 0)
    {
        body = "{\"sources\":[{\"sourceReference\":2147483648}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "fraction") == 0)
    {
        body = "{\"sources\":[{\"sourceReference\":1.5}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "boolean") == 0)
    {
        body = "{\"sources\":[{\"sourceReference\":true}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "nul") == 0)
    {
        body = "{\"sources\":[{\"name\":\"a\\u0000b\"}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "array") == 0)
    {
        body = "{\"sources\":{}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "missing") == 0)
    {
        body = "{}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "object") == 0)
    {
        body = "{\"sources\":[false]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "related-object") == 0)
    {
        body = "{\"sources\":[{\"sources\":[false]}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "duplicate-key") == 0)
    {
        body = "{\"sources\":[{\"name\":\"a\",\"n\\u0061me\":\"b\"}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "long-name") == 0 || strcmp(mode, "capacity") == 0)
    {
    }
    else
        return 2;
    int failed = 0;
    char json[8192], generated[4096];
    UmiDebugSourceCatalog *catalog = malloc(sizeof *catalog), *before = malloc(sizeof *before);
    CHECK(catalog != NULL && before != NULL);
    memset(catalog, 0xA5, sizeof *catalog);
    *before = *catalog;
    if (strcmp(mode, "long-name") == 0)
    {
        char name[300];
        memset(name, 'x', sizeof name - 1U);
        name[sizeof name - 1U] = '\0';
        snprintf(generated, sizeof generated, "{\"sources\":[{\"name\":\"%s\"}]}", name);
        body = generated;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "capacity") == 0)
    {
        strcpy(generated, "{\"sources\":[");
        for (size_t i = 0U; i <= UMI_DEBUG_SOURCE_CATALOG_LIMIT; ++i)
            strcat(generated, i ? ",{}" : "{}");
        strcat(generated, "]}");
        body = generated;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    snprintf(json, sizeof json,
             "{\"type\":\"response\",\"command\":\"loadedSources\",\"success\":true,\"body\":%s}",
             body);
    CHECK(UmiDebugSourceCatalogDecode(json, catalog) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(catalog, before, sizeof *catalog) == 0);
    else
    {
        CHECK(catalog->connection.generation == 0U && catalog->connection.session_id[0] == '\0');
        if (strcmp(mode, "normal") == 0 || strcmp(mode, "escaped-keys") == 0)
            CHECK(catalog->items[0].reference == 42U);
        if (strcmp(mode, "optional") == 0)
            CHECK(catalog->count == 1U && catalog->items[0].reference == 0U);
        if (strcmp(mode, "repeated") == 0)
            CHECK(catalog->count == 2U);
        if (strcmp(mode, "related") == 0)
            CHECK(catalog->items[0].related_count == 1U);
        if (strcmp(mode, "unicode") == 0)
            CHECK(strcmp(catalog->items[0].name, "caf\xc3\xa9.c") == 0);
    }
done:
    free(catalog);
    free(before);
    return failed;
}
