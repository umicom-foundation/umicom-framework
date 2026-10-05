/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_pull_diagnostic_catalogue.c
 * PURPOSE: Check full diagnostic response ownership, correlation and complete-report boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/diagnostic_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                                         \
    do                                                                                                       \
    {                                                                                                        \
        if (!(value))                                                                                        \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                      \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"full",
                           "empty",
                           "result-id",
                           "no-result-id",
                           "empty-id",
                           "unicode-id",
                           "id-type",
                           "duplicate-id",
                           "unchanged",
                           "unknown-kind",
                           "missing-kind",
                           "kind-type",
                           "missing-items",
                           "items-type",
                           "invalid-row",
                           "related-empty",
                           "related-present",
                           "related-type",
                           "duplicate-items",
                           "duplicate-kind",
                           "wrong-id",
                           "remote-error",
                           "null-result",
                           "protocol",
                           "method",
                           "duplicate-result",
                           "owned",
                           "raw-data",
                           "uri-invalid",
                           "uri-null",
                           "cancel",
                           "arguments"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    const char *items = "[{\"message\":\"Unknown "
                        "name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":2}},\"data\":{\"opaque\":[3,\"same provider\"]}}]";
    const char *kind = "\"kind\":\"full\",", *extra = "", *uri = "file:///workspace/main.c";
    UmiStatus expected = UMI_STATUS_OK;
    uint64_t id = 7U;
    if (strcmp(mode, "empty") == 0)
        items = "[]";
    if (strcmp(mode, "result-id") == 0)
        extra = ",\"resultId\":\"report-key\"";
    if (strcmp(mode, "empty-id") == 0)
        extra = ",\"resultId\":\"\"";
    if (strcmp(mode, "unicode-id") == 0)
        extra = ",\"resultId\":\"caf\u00e9\"";
    if (strcmp(mode, "id-type") == 0)
    {
        extra = ",\"resultId\":null";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "duplicate-id") == 0)
    {
        extra = ",\"resultId\":\"a\",\"resultId\":\"b\"";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "unchanged") == 0)
    {
        kind = "\"kind\":\"unchanged\",";
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "unknown-kind") == 0)
    {
        kind = "\"kind\":\"partial\",";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "missing-kind") == 0)
    {
        kind = "";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "kind-type") == 0)
    {
        kind = "\"kind\":1,";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "items-type") == 0)
    {
        items = "null";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "invalid-row") == 0)
    {
        items = "[{\"message\":\"No range\"}]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "related-empty") == 0)
        extra = ",\"relatedDocuments\":{}";
    if (strcmp(mode, "related-present") == 0)
    {
        extra = ",\"relatedDocuments\":{\"file:///other.c\":{\"kind\":\"full\",\"items\":[]}}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "related-type") == 0)
    {
        extra = ",\"relatedDocuments\":[]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "duplicate-items") == 0)
    {
        extra = ",\"items\":[]";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "duplicate-kind") == 0)
    {
        extra = ",\"kind\":\"full\"";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "wrong-id") == 0)
    {
        id = 8U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "uri-invalid") == 0)
    {
        uri = "not a URI";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "uri-null") == 0)
    {
        uri = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    char response[4096];
    int written =
        snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{%s\"items\":%s%s}}",
                 kind, items, extra);
    CHECK(written > 0 && (size_t)written < sizeof(response));
    if (strcmp(mode, "missing-items") == 0)
    {
        strcpy(response, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"kind\":\"full\"}}");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "remote-error") == 0)
    {
        strcpy(response, "{\"jsonrpc\":\"2.0\",\"id\":7,\"error\":{\"code\":-32603,\"message\":\"failed\"}}");
        expected = UMI_STATUS_UNAVAILABLE;
    }
    if (strcmp(mode, "null-result") == 0)
    {
        strcpy(response, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":null}");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "protocol") == 0)
    {
        strcpy(response, "{\"jsonrpc\":\"1.0\",\"id\":7,\"result\":{}}");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "method") == 0)
    {
        strcpy(response, "{\"jsonrpc\":\"2.0\",\"id\":7,\"method\":\"x\",\"result\":{}}");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "duplicate-result") == 0)
    {
        strcpy(response, "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{},\"result\":{}}");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    if (strcmp(mode, "arguments") == 0)
    {
        CHECK(UmiLanguageDiagnosticCatalogueReadPullResponse(response, strlen(response), id, uri, NULL,
                                                             NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLanguageDiagnosticCatalogueReadPullResponse(NULL, 0U, id, uri, NULL, &catalogue) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              catalogue == NULL);
        const char *value = "sentinel";
        CHECK(UmiLanguageDiagnosticCataloguePullResultId(NULL, &value) == UMI_STATUS_INVALID_ARGUMENT &&
              value == NULL);
    }
    CHECK(UmiLanguageDiagnosticCatalogueReadPullResponse(response, strlen(response), id, uri, cancel,
                                                         &catalogue) == expected);
    if (expected == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : 1U;
        CHECK(UmiLanguageDiagnosticCatalogueCount(catalogue) == count);
        if (strcmp(mode, "owned") == 0)
            memset(response, 'x', strlen(response));
        UmiLanguageDiagnosticPublication publication;
        CHECK(UmiLanguageDiagnosticCataloguePublication(catalogue, &publication) == UMI_STATUS_OK);
        CHECK(strcmp(publication.uri, uri) == 0 && !publication.has_version);
        const char *result_id = "sentinel";
        UmiStatus result_status = UmiLanguageDiagnosticCataloguePullResultId(catalogue, &result_id);
        if (strcmp(mode, "result-id") == 0 || strcmp(mode, "empty-id") == 0 ||
            strcmp(mode, "unicode-id") == 0)
        {
            CHECK(result_status == UMI_STATUS_OK);
            CHECK(strcmp(result_id, strcmp(mode, "result-id") == 0  ? "report-key"
                                    : strcmp(mode, "empty-id") == 0 ? ""
                                                                    : "caf\xc3\xa9") == 0);
        }
        else
            CHECK(result_status == UMI_STATUS_NOT_FOUND && result_id == NULL);
        if (count != 0U)
        {
            UmiLanguageDiagnostic row;
            CHECK(UmiLanguageDiagnosticCatalogueAt(catalogue, 0U, &row) == UMI_STATUS_OK);
            CHECK(strcmp(row.message, "Unknown name") == 0 && row.has_data);
            const char *raw = NULL;
            size_t bytes = 0U;
            CHECK(UmiLanguageDiagnosticCatalogueItemJson(catalogue, 0U, &raw, &bytes) == UMI_STATUS_OK);
            CHECK(bytes == strlen(items) - 2U && memcmp(raw, items + 1U, bytes) == 0);
            CHECK(UmiLanguageDiagnosticCatalogueValidateSource(catalogue, uri, NULL, "ab", 2U, NULL) ==
                  UMI_STATUS_OK);
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
