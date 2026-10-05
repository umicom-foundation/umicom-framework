/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connection_checks/test_catalogue.c
 * PURPOSE: Reject malformed catalogues while distinguishing a missing model from parse failure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/provider_connection_checks/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"line %d: %s\n",__LINE__,#c); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *body = "{\"object\":\"list\",\"data\":[{\"id\":\"test-model\",\"object\":\"model\",\"owned_by\":\"example\"}]}";
    const char *model = "test-model"; UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(argv[1], "valid") == 0) {}
    else if (strcmp(argv[1], "missing") == 0) { model = "missing-model"; expected = UMI_STATUS_NOT_FOUND; }
    else if (strcmp(argv[1], "empty") == 0) { body = "{\"object\":\"list\",\"data\":[]}"; expected = UMI_STATUS_NOT_FOUND; }
    else if (strcmp(argv[1], "unicode") == 0) { body = "{\"object\":\"list\",\"data\":[{\"id\":\"caf\\u00e9\",\"object\":\"model\"}]}"; model = "caf\xc3\xa9"; }
    else if (strcmp(argv[1], "duplicate-data") == 0) { body = "{\"object\":\"list\",\"data\":[],\"data\":[]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(argv[1], "duplicate-id") == 0) { body = "{\"object\":\"list\",\"data\":[{\"id\":\"test-model\",\"i\\u0064\":\"other\",\"object\":\"model\"}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(argv[1], "trailing") == 0) { body = "{\"object\":\"list\",\"data\":[]} false"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(argv[1], "nul") == 0) { body = "{\"object\":\"list\",\"data\":[{\"id\":\"test\\u0000model\",\"object\":\"model\"}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(argv[1], "bad-id") == 0) { body = "{\"object\":\"list\",\"data\":[{\"id\":2,\"object\":\"model\"}]}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(argv[1], "malformed") == 0) { body = "{\"object\":\"list\",\"data\":[}"; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(argv[1], "capacity") == 0) {
        UmiProviderConnectionCheckResult result = {0};
        CHECK(UmiConnectionCheckDecode(body, UMI_CONNECTION_CHECK_BODY_CAPACITY, model, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    } else return 2;
    UmiProviderConnectionCheckResult result = {123U, 42U, false};
    CHECK(UmiConnectionCheckDecode(body, strlen(body), model, &result) == expected);
    if (expected == UMI_STATUS_OK) CHECK(result.model_listed && result.listed_models == 1U);
    else if (expected == UMI_STATUS_NOT_FOUND) CHECK(!result.model_listed && result.listed_models == (strcmp(argv[1], "empty") == 0 ? 0U : 1U));
    else CHECK(result.http_status == 123U && result.listed_models == 42U && !result.model_listed);
    return 0;
}
