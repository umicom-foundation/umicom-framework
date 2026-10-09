/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_exceptions/test_catalog.c
 * PURPOSE: Exercise bounded adapter exception metadata, UTF-8 and unchanged failure outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/exception_filters.h"
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
    const char *mode = argv[1], *filters = NULL;
    bool good = false;
    size_t count = 0U;
    if (strcmp(mode, "normal") == 0)
    {
        filters = "[{\"filter\":\"caught\",\"label\":\"Caught\",\"default\":true},{\"filter\":"
                  "\"uncaught\",\"label\":\"Uncaught\"}]";
        good = true;
        count = 2U;
    }
    else if (strcmp(mode, "escaped-keys") == 0)
    {
        filters =
            "[{\"\\u0066ilter\":\"caught\",\"\\u006cabel\":\"Caught\",\"\\u0064efault\":true}]";
        good = true;
        count = 1U;
    }
    else if (strcmp(mode, "empty") == 0)
    {
        filters = "[]";
        good = true;
    }
    else if (strcmp(mode, "missing") == 0)
    {
        filters = NULL;
        good = true;
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        filters = "[{\"filter\":\"unicode\",\"label\":\"caf\\u00e9 \\ud83d\\ude80\"}]";
        good = true;
        count = 1U;
    }
    else if (strcmp(mode, "unknown") == 0)
    {
        filters = "[{\"filter\":\"f\",\"label\":\"Filter\",\"supportsCondition\":true,\"future\":{"
                  "\"value\":1}}]";
        good = true;
        count = 1U;
    }
    else if (strcmp(mode, "duplicate-id") == 0)
        filters =
            "[{\"filter\":\"f\",\"label\":\"First\"},{\"filter\":\"f\",\"label\":\"Second\"}]";
    else if (strcmp(mode, "duplicate-key") == 0)
        filters = "[{\"filter\":\"f\",\"filter\":\"g\",\"label\":\"Filter\"}]";
    else if (strcmp(mode, "escaped-duplicate") == 0)
        filters = "[{\"filter\":\"f\",\"\\u0066ilter\":\"g\",\"label\":\"Filter\"}]";
    else if (strcmp(mode, "bad-label") == 0)
        filters = "[{\"filter\":\"f\",\"label\":3}]";
    else if (strcmp(mode, "nul") == 0)
        filters = "[{\"filter\":\"f\\u0000hidden\",\"label\":\"Filter\"}]";
    else if (strcmp(mode, "surrogate") == 0)
        filters = "[{\"filter\":\"f\",\"label\":\"\\ud800\"}]";
    else if (strcmp(mode, "empty-id") == 0)
        filters = "[{\"filter\":\"\",\"label\":\"Filter\"}]";
    else if (strcmp(mode, "default-type") == 0)
        filters = "[{\"filter\":\"f\",\"label\":\"Filter\",\"default\":\"true\"}]";
    else if (strcmp(mode, "array-type") == 0)
        filters = "{}";
    else if (strcmp(mode, "refuse") == 0 || strcmp(mode, "command") == 0 ||
             strcmp(mode, "body") == 0)
        filters = "[]";
    else if (strcmp(mode, "capacity") != 0 && strcmp(mode, "long-id") != 0)
        return 2;
    int failed = 0;
    UmiDebugExceptionCatalog *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    char json[16384], generated[8192];
    CHECK(out != NULL && before != NULL);
    memset(out, 0xA5, sizeof *out);
    *before = *out;
    if (strcmp(mode, "capacity") == 0)
    {
        size_t used = 0U;
        generated[used++] = '[';
        for (unsigned i = 0U; i < 33U; ++i)
        {
            int written = snprintf(generated + used, sizeof generated - used,
                                   "%s{\"filter\":\"f%u\",\"label\":\"F\"}", i ? "," : "", i);
            CHECK(written > 0 && (size_t)written < sizeof generated - used);
            used += (size_t)written;
        }
        generated[used++] = ']';
        generated[used] = '\0';
        filters = generated;
    }
    else if (strcmp(mode, "long-id") == 0)
    {
        char id[180];
        memset(id, 'a', sizeof id - 1U);
        id[sizeof id - 1U] = '\0';
        snprintf(generated, sizeof generated, "[{\"filter\":\"%s\",\"label\":\"F\"}]", id);
        filters = generated;
    }
    if (filters == NULL)
        snprintf(json, sizeof json,
                 "{\"type\":\"response\",\"command\":\"initialize\",\"success\":true,\"body\":{}}");
    else
        snprintf(json, sizeof json,
                 "{\"type\":\"response\",\"command\":\"%s\",\"success\":%s,\"body\":{"
                 "\"exceptionBreakpointFilters\":%s}}",
                 strcmp(mode, "command") == 0 ? "launch" : "initialize",
                 strcmp(mode, "refuse") == 0 ? "false" : "true", filters);
    if (strcmp(mode, "body") == 0)
        strcpy(json,
               "{\"type\":\"response\",\"command\":\"initialize\",\"success\":true,\"body\":null}");
    UmiStatus status = UmiDebugExceptionCatalogDecode(json, out);
    if (good)
    {
        CHECK(status == UMI_STATUS_OK && out->count == count);
        if (strcmp(mode, "escaped-keys") == 0)
            CHECK(out->items[0].default_enabled == 1);
        if (strcmp(mode, "normal") == 0)
            CHECK(out->items[0].default_enabled == 1 && out->items[1].default_enabled == 0);
        if (strcmp(mode, "unicode") == 0)
            CHECK(strcmp(out->items[0].label, "caf\xc3\xa9 \xf0\x9f\x9a\x80") == 0);
    }
    else
        CHECK(status != UMI_STATUS_OK && memcmp(out, before, sizeof *out) == 0);
done:
    free(out);
    free(before);
    return failed;
}
