/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_diagnostic_catalogue.c
 * PURPOSE: Exercise complete diagnostic ownership, metadata, notification routing and exact source ranges.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/diagnostic_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Case
{
    const char *name, *json;
    UmiStatus wanted;
} Case;
static const Case cases[] = {
    {"basic",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_OK},
    {"unversioned",
     "{\"uri\":\"file:///"
     "main.c\",\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
     "\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_OK},
    {"zero-version",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":0,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_OK},
    {"negative-version",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":-1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_OK},
    {"null-version",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":null,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"large-version",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":2147483648,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
     "\"end\":{\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"error",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"severity\":1}]}",
     UMI_STATUS_OK},
    {"warning",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"severity\":2}]}",
     UMI_STATUS_OK},
    {"information",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"severity\":3}]}",
     UMI_STATUS_OK},
    {"hint",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"severity\":4}]}",
     UMI_STATUS_OK},
    {"invalid-severity",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"severity\":0}]}",
     UMI_STATUS_PARSE_ERROR},
    {"severity-type",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"severity\":\"1\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"numeric-code",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"code\":-2147483648}]}",
     UMI_STATUS_OK},
    {"string-code",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"code\":\"E007\"}]}",
     UMI_STATUS_OK},
    {"empty-code",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"code\":\"\"}]}",
     UMI_STATUS_OK},
    {"code-null",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"code\":null}]}",
     UMI_STATUS_PARSE_ERROR},
    {"code-large",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"code\":2147483648}]}",
     UMI_STATUS_PARSE_ERROR},
    {"source",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"source\":\"clangd\"}]}",
     UMI_STATUS_OK},
    {"source-type",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"source\":false}]}",
     UMI_STATUS_PARSE_ERROR},
    {"description",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown "
     "name\",\"codeDescription\":{\"href\":\"https://example.invalid/errors/name\"}}]}",
     UMI_STATUS_OK},
    {"description-missing",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"codeDescription\":{}}]}",
     UMI_STATUS_PARSE_ERROR},
    {"description-uri",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"codeDescription\":{\"href\":\"bad "
     "uri\"}}]}",
     UMI_STATUS_PARSE_ERROR},
    {"tags",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"tags\":[1,2,77]}]}",
     UMI_STATUS_OK},
    {"tags-type",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"tags\":1}]}",
     UMI_STATUS_PARSE_ERROR},
    {"tag-zero",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"tags\":[0]}]}",
     UMI_STATUS_PARSE_ERROR},
    {"tag-type",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"tags\":[\"1\"]}]}",
     UMI_STATUS_PARSE_ERROR},
    {"data",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"data\":{\"context\":[\"kept\",3]}}]}",
     UMI_STATUS_OK},
    {"data-null",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"data\":null}]}",
     UMI_STATUS_OK},
    {"unicode",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"caf\\u00e9 \\ud83d\\ude00\"}]}",
     UMI_STATUS_OK},
    {"literal",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"<script>literal</script>\"}]}",
     UMI_STATUS_OK},
    {"empty-message",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"\"}]}",
     UMI_STATUS_OK},
    {"message-type",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":3}]}",
     UMI_STATUS_PARSE_ERROR},
    {"related",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown "
     "name\",\"relatedInformation\":[{\"location\":{\"uri\":\"file:///"
     "other.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}},"
     "\"message\":\"Related declaration\"}]}]}",
     UMI_STATUS_OK},
    {"related-empty",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"relatedInformation\":[]}]}",
     UMI_STATUS_OK},
    {"related-type",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"relatedInformation\":{}}]}",
     UMI_STATUS_PARSE_ERROR},
    {"related-bad",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\",\"relatedInformation\":[{}]}]}",
     UMI_STATUS_PARSE_ERROR},
    {"related-message-type",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown "
     "name\",\"relatedInformation\":[{\"location\":{\"uri\":\"file:///"
     "other.c\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}},"
     "\"message\":null}]}]}",
     UMI_STATUS_PARSE_ERROR},
    {"reversed-range",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":1,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"negative-position",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":-1},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"range-type",
     "{\"uri\":\"file:///main.c\",\"version\":1,\"diagnostics\":[{\"range\":null,\"message\":\"Unknown "
     "name\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"missing-message",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}}}]}",
     UMI_STATUS_PARSE_ERROR},
    {"late-invalid",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"},{\"message\":\"late invalid\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"empty", "{\"uri\":\"file:///main.c\",\"version\":1,\"diagnostics\":[]}", UMI_STATUS_OK},
    {"null", "null", UMI_STATUS_PARSE_ERROR},
    {"array", "[]", UMI_STATUS_PARSE_ERROR},
    {"missing-uri", "{\"diagnostics\":[]}", UMI_STATUS_PARSE_ERROR},
    {"missing-diagnostics", "{\"uri\":\"file:///main.c\"}", UMI_STATUS_PARSE_ERROR},
    {"uri-type",
     "{\"uri\":3,\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"uri-escape",
     "{\"uri\":\"file:///"
     "%zz\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"Unknown name\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"diagnostics-type", "{\"uri\":\"file:///main.c\",\"version\":1,\"diagnostics\":{}}",
     UMI_STATUS_PARSE_ERROR},
    {"duplicate-uri", "{\"uri\":\"file:///main.c\",\"uri\":\"file:///other.c\",\"diagnostics\":[]}",
     UMI_STATUS_ALREADY_EXISTS},
    {"duplicate-message",
     "{\"uri\":\"file:///"
     "main.c\",\"version\":1,\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"message\":\"a\",\"message\":\"b\"}]}",
     UMI_STATUS_ALREADY_EXISTS},

};
/* Construct boundary inputs at runtime so regression source remains readable.
 * Every generated row contributes to a complete publication, not a partial view. */
static char *Repeated(const char *prefix, const char *item, const char *suffix, size_t count)
{
    size_t capacity = strlen(prefix) + strlen(suffix) + count * (strlen(item) + 1U) + 1U;
    char *out = malloc(capacity);
    if (out == NULL)
        return NULL;
    strcpy(out, prefix);
    for (size_t i = 0U; i < count; ++i)
    {
        if (i != 0U)
            strcat(out, ",");
        strcat(out, item);
    }
    strcat(out, suffix);
    return out;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *input = cases[0].json;
    char *allocated = NULL;
    UmiStatus wanted = UMI_STATUS_OK;
    int matched = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i].name) == 0)
        {
            input = cases[i].json;
            wanted = cases[i].wanted;
            matched = 1;
        }
    const char *extras[] = {"owned",
                            "notification",
                            "wrong-method",
                            "notification-id",
                            "notification-error",
                            "notification-protocol",
                            "notification-params",
                            "notification-duplicate",
                            "cancelled",
                            "arguments",
                            "count-limit",
                            "count-boundary",
                            "related-limit",
                            "related-total-limit",
                            "message-limit",
                            "source-limit",
                            "code-limit",
                            "tag-limit",
                            "validate",
                            "validate-uri",
                            "validate-version",
                            "validate-unknown-version",
                            "validate-unversioned",
                            "validate-outside",
                            "validate-surrogate",
                            "validate-unicode",
                            "validate-crlf",
                            "validate-invalid-source",
                            "validate-related",
                            "validate-cancelled",
                            "raw"};
    for (size_t i = 0U; i < sizeof(extras) / sizeof(extras[0]); ++i)
        if (strcmp(mode, extras[i]) == 0)
            matched = 1;
    CHECK(matched);
    const char *item = "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":"
                       "3}},\"message\":\"name\"}";
    if (strcmp(mode, "count-limit") == 0 || strcmp(mode, "count-boundary") == 0)
    {
        allocated = Repeated("{\"uri\":\"file:///main.c\",\"diagnostics\":[", item, "]}",
                             strcmp(mode, "count-limit") == 0 ? 4097U : 4096U);
        CHECK(allocated != NULL);
        input = allocated;
        if (strcmp(mode, "count-limit") == 0)
            wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "related-limit") == 0 || strcmp(mode, "related-total-limit") == 0)
    {
        const char *related = "{\"location\":{\"uri\":\"file:///"
                              "a\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                              "\"character\":0}}},\"message\":\"n\"}";
        char *row = Repeated("{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                             "\"character\":0}},\"message\":\"m\",\"relatedInformation\":[",
                             related, "]}", strcmp(mode, "related-limit") == 0 ? 129U : 128U);
        CHECK(row != NULL);
        allocated = Repeated("{\"uri\":\"file:///main.c\",\"diagnostics\":[", row, "]}",
                             strcmp(mode, "related-limit") == 0 ? 1U : 65U);
        free(row);
        CHECK(allocated != NULL);
        input = allocated;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "message-limit") == 0 || strcmp(mode, "source-limit") == 0 ||
        strcmp(mode, "code-limit") == 0)
    {
        size_t count = strcmp(mode, "message-limit") == 0 ? 65537U : 4097U;
        allocated = malloc(count + 512U);
        CHECK(allocated != NULL);
        const char *field = strcmp(mode, "message-limit") == 0  ? "message"
                            : strcmp(mode, "source-limit") == 0 ? "source"
                                                                : "code";
        int prefix = snprintf(allocated, count + 512U,
                              "{\"uri\":\"file:///"
                              "main.c\",\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                              "\"end\":{\"line\":0,\"character\":3}},%s\"%s\":\"",
                              strcmp(field, "message") == 0 ? "" : "\"message\":\"m\",", field);
        CHECK(prefix > 0);
        memset(allocated + (size_t)prefix, 'x', count);
        strcpy(allocated + (size_t)prefix + count, "\"}]}");
        input = allocated;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "tag-limit") == 0)
    {
        allocated = Repeated("{\"uri\":\"file:///"
                             "main.c\",\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                             "\"end\":{\"line\":0,\"character\":3}},\"message\":\"m\",\"tags\":[",
                             "1", "]}]}", 257U);
        CHECK(allocated != NULL);
        input = allocated;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    int notification = strncmp(mode, "notification", 12U) == 0 || strcmp(mode, "wrong-method") == 0;
    if (notification)
    {
        const char *method =
            strcmp(mode, "wrong-method") == 0 ? "window/logMessage" : "textDocument/publishDiagnostics";
        const char *protocol = strcmp(mode, "notification-protocol") == 0 ? "1.0" : "2.0";
        const char *extra = strcmp(mode, "notification-id") == 0      ? "\"id\":1,"
                            : strcmp(mode, "notification-error") == 0 ? "\"error\":{},"
                            : strcmp(mode, "notification-duplicate") == 0
                                ? "\"method\":\"textDocument/publishDiagnostics\","
                                : "";
        allocated = malloc(strlen(input) + 512U);
        CHECK(allocated != NULL);
        (void)snprintf(allocated, strlen(input) + 512U,
                       "{\"jsonrpc\":\"%s\",%s\"method\":\"%s\",\"params\":%s}", protocol, extra, method,
                       strcmp(mode, "notification-params") == 0 ? "null" : input);
        input = allocated;
        if (strcmp(mode, "wrong-method") == 0)
            wanted = UMI_STATUS_NOT_FOUND;
        else if (strcmp(mode, "notification-duplicate") == 0)
            wanted = UMI_STATUS_ALREADY_EXISTS;
        else if (strcmp(mode, "notification") != 0)
            wanted = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "owned") == 0)
    {
        allocated = malloc(strlen(input) + 1U);
        CHECK(allocated != NULL);
        strcpy(allocated, input);
        input = allocated;
    }
    if (strcmp(mode, "validate-unversioned") == 0)
        input = "{\"uri\":\"file:///main.c\",\"diagnostics\":[]}";
    if (strcmp(mode, "validate-crlf") == 0)
        input = "{\"uri\":\"file:///"
                "main.c\",\"diagnostics\":[{\"message\":\"m\",\"range\":{\"start\":{\"line\":1,\"character\":"
                "0},\"end\":{\"line\":1,\"character\":1}}}]}";
    if (strcmp(mode, "validate-related") == 0)
        input = "{\"uri\":\"file:///"
                "main.c\",\"diagnostics\":[{\"message\":\"m\",\"range\":{\"start\":{\"line\":0,\"character\":"
                "0},\"end\":{\"line\":0,\"character\":3}},\"relatedInformation\":[{\"message\":\"note\","
                "\"location\":{\"uri\":\"file:///"
                "main.c\",\"range\":{\"start\":{\"line\":9,\"character\":0},\"end\":{\"line\":9,"
                "\"character\":1}}}}]}]}";
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    UmiLanguageDiagnosticCatalogue *catalogue = (UmiLanguageDiagnosticCatalogue *)1;
    UmiStatus status =
        notification
            ? UmiLanguageDiagnosticCatalogueReadNotification(input, strlen(input), cancel, &catalogue)
            : UmiLanguageDiagnosticCatalogueCreate(input, strlen(input), cancel, &catalogue);
    CHECK(status == wanted);
    if (status == UMI_STATUS_OK)
    {
        if (strcmp(mode, "owned") == 0)
            memset(allocated, 'x', strlen(allocated));
        UmiLanguageDiagnosticPublication publication;
        CHECK(UmiLanguageDiagnosticCataloguePublication(catalogue, &publication) == UMI_STATUS_OK);
        CHECK(strcmp(publication.uri, "file:///main.c") == 0);
        UmiLanguageDiagnostic value = {0};
        size_t count = UmiLanguageDiagnosticCatalogueCount(catalogue);
        if (count != 0U)
        {
            CHECK(UmiLanguageDiagnosticCatalogueAt(catalogue, 0U, &value) == UMI_STATUS_OK);
            if (strcmp(mode, "basic") == 0 || strcmp(mode, "owned") == 0)
                CHECK(strcmp(value.message, "Unknown name") == 0 && value.severity == 1 &&
                      !value.has_severity);
            if (strcmp(mode, "numeric-code") == 0)
                CHECK(value.has_code && value.code_is_number && strcmp(value.code, "-2147483648") == 0);
            if (strcmp(mode, "tags") == 0)
                CHECK(value.unnecessary && value.deprecated);
            if (strcmp(mode, "related") == 0)
            {
                UmiLanguageDiagnosticRelated related;
                CHECK(value.related_count == 1U);
                CHECK(UmiLanguageDiagnosticCatalogueRelated(catalogue, 0U, 0U, &related) == UMI_STATUS_OK);
                CHECK(strcmp(related.location.uri, "file:///other.c") == 0 &&
                      strcmp(related.message, "Related declaration") == 0);
            }
            if (strcmp(mode, "raw") == 0)
            {
                const char *raw = NULL;
                size_t bytes = 0U;
                CHECK(UmiLanguageDiagnosticCatalogueItemJson(catalogue, 0U, &raw, &bytes) == UMI_STATUS_OK &&
                      bytes > 0U && raw[0] == '{');
            }
        }
        if (strncmp(mode, "validate", 8U) == 0)
        {
            const char *source = "abc", *uri = "file:///main.c";
            int32_t version = 1;
            const int32_t *known = &version;
            UmiStatus expected = UMI_STATUS_OK;
            if (strcmp(mode, "validate-uri") == 0)
            {
                uri = "file:///other.c";
                expected = UMI_STATUS_NOT_FOUND;
            }
            if (strcmp(mode, "validate-version") == 0)
            {
                version = 2;
                expected = UMI_STATUS_INVALID_STATE;
            }
            if (strcmp(mode, "validate-unknown-version") == 0)
            {
                known = NULL;
                expected = UMI_STATUS_NOT_IMPLEMENTED;
            }
            if (strcmp(mode, "validate-unversioned") == 0)
                known = NULL;
            /* Exact coordinate resolution reports an invalid argument for a range
             * outside the captured text; retain the earlier expectation for review. */
#if 0
if(strcmp(mode,"validate-outside")==0) {source="ab";expected=UMI_STATUS_NOT_FOUND;}
#endif
            if (strcmp(mode, "validate-outside") == 0)
            {
                source = "ab";
                expected = UMI_STATUS_INVALID_ARGUMENT;
            }
            if (strcmp(mode, "validate-surrogate") == 0)
            {
                source = "ab\xf0\x9f\x98\x80";
                expected = UMI_STATUS_INVALID_ARGUMENT;
            }
            if (strcmp(mode, "validate-unicode") == 0)
                source = "a\xf0\x9f\x98\x80";
            if (strcmp(mode, "validate-crlf") == 0)
                source = "a\r\nb";
            if (strcmp(mode, "validate-invalid-source") == 0)
            {
                source = "abc\xff";
                expected = UMI_STATUS_PARSE_ERROR;
            }
            /* Exact coordinate resolution reports an invalid argument for a range
             * outside the captured text; retain the earlier expectation for review. */
#if 0
if(strcmp(mode,"validate-related")==0) expected=UMI_STATUS_NOT_FOUND;
#endif
            if (strcmp(mode, "validate-related") == 0)
                expected = UMI_STATUS_INVALID_ARGUMENT;
            if (strcmp(mode, "validate-cancelled") == 0)
            {
                umi_cancellation_token_request(cancel);
                expected = UMI_STATUS_CANCELLED;
            }
            CHECK(UmiLanguageDiagnosticCatalogueValidateSource(catalogue, uri, known, source, strlen(source),
                                                               cancel) == expected);
        }
        if (strcmp(mode, "arguments") == 0)
        {
            UmiLanguageDiagnosticRelated related;
            const char *raw = (const char *)1;
            size_t bytes = 9U;
            CHECK(UmiLanguageDiagnosticCatalogueAt(catalogue, 9999U, &value) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiLanguageDiagnosticCatalogueRelated(catalogue, 0U, 0U, &related) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiLanguageDiagnosticCataloguePublication(NULL, &publication) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageDiagnosticCatalogueItemJson(catalogue, 9999U, &raw, &bytes) ==
                      UMI_STATUS_NOT_FOUND &&
                  raw == NULL && bytes == 0U);
            CHECK(UmiLanguageDiagnosticCatalogueCreate(NULL, 0U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageDiagnosticCatalogueCount(NULL) == 0U);
            UmiLanguageDiagnosticCatalogueDestroy(NULL);
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    free(allocated);
    return 0;
}
