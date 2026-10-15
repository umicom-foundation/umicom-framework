/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_request.c
 * PURPOSE: Verify editable API requests, validation and environment expansion.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../value_archive/revision_cases.h"
#include "../value_archive/construction_cases.h"
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/request.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Reusing a request must not retain a previous query or publish half a target. */
static int RequestTargetReplacementCases(void)
{
    UmiWebRequest *value = calloc(1U, sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    char *oversized = malloc(UMI_WEB_QUERY_CAPACITY + 16U);
    int result = 1;
    if (value == NULL || before == NULL || oversized == NULL) goto construction_cleanup;
    umi_web_request_init(value);
    UMI_CONSTRUCTION_REQUIRE(umi_web_request_set_target(value, "/old?accepted=yes") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    memcpy(oversized, "/new?", 5U);
    memset(oversized + 5U, 'x', UMI_WEB_QUERY_CAPACITY);
    oversized[5U + UMI_WEB_QUERY_CAPACITY] = '\0';
    UMI_CONSTRUCTION_REQUIRE(umi_web_request_set_target(value, oversized) == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_web_request_set_target(value, value->path) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->path, "/old") == 0 && value->query[0] == '\0');
    /* The new path and query can also come from the old query buffer. */
    strcpy(value->query, "/borrowed?a=b");
    UMI_CONSTRUCTION_REQUIRE(umi_web_request_set_target(value, value->query) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->path, "/borrowed") == 0 && strcmp(value->query, "a=b") == 0);
    result = 0;
construction_cleanup:
    free(oversized);
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (RequestTargetReplacementCases() != 0) return 1;

    UmiWebWorkbenchRequest request;
    UmiWebWorkbenchRequest resolved;
    UmiWebWorkbenchEnvironment environment;
    size_t unresolved = 0U;
    char message[128U];
    umi_web_workbench_request_init(&request, "orders", "Orders", UMI_HTTP_METHOD_POST,
        "{{baseUrl}}/orders");
    assert(umi_web_workbench_request_set_header(&request, "Accept", "application/json") == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiWebWorkbenchRequest, &request,
        (umi_web_workbench_request_set_header(&request, "Accept", "application/json")));

    assert(umi_web_workbench_request_set_body(&request, UMI_WEB_WORKBENCH_BODY_JSON,
        "{\"book\":\"{{book}}\"}") == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiWebWorkbenchRequest, &request,
        (umi_web_workbench_request_set_body(&request, UMI_WEB_WORKBENCH_BODY_JSON,
        "{\"book\":\"{{book}}\"}")));

    umi_web_workbench_environment_init(&environment, "dev", "Development");
    assert(umi_web_workbench_environment_set(&environment, "baseUrl",
        "https://api.example.test", true) == UMI_STATUS_OK);
    assert(umi_web_workbench_environment_set(&environment, "book", "LONDON", true) == UMI_STATUS_OK);
    assert(umi_web_workbench_request_resolve(&request, &environment, &resolved,
        &unresolved) == UMI_STATUS_OK);
    assert(unresolved == 0U);
    assert(strcmp(resolved.url, "https://api.example.test/orders") == 0);
    assert(strstr(resolved.body, "LONDON") != NULL);
    assert(umi_web_workbench_request_validate(&resolved, message, sizeof(message)) == UMI_STATUS_OK);
    assert(umi_web_workbench_request_remove_header(&resolved, "Accept") == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiWebWorkbenchRequest, &resolved,
        (umi_web_workbench_request_remove_header(&resolved, "Accept")));

    /* A damaged header count must be rejected before any fixed header slot is accessed. */
    resolved.header_count = UMI_WEB_MAX_HEADERS + 1U;
    assert(umi_web_workbench_request_set_header(&resolved, "X-Test", "value") ==
        UMI_STATUS_INVALID_STATE);
    assert(umi_web_workbench_request_header(&resolved, "X-Test") == NULL);
    return 0;
}
