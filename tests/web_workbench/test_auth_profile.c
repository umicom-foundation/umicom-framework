/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_auth_profile.c
 *
 * PURPOSE:
 *   Verify secret-reference profiles and transient authentication use.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../value_archive/revision_cases.h"
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/auth_profile.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWebWorkbenchAuthCatalogue catalogue;
    UmiWebWorkbenchAuthProfile bearer;
    UmiWebWorkbenchAuthProfile query;
    UmiWebWorkbenchRequest request;
    umi_web_workbench_auth_catalogue_init(&catalogue);
    umi_web_workbench_auth_profile_init(&bearer, "oauth", "OAuth",
        UMI_WEB_WORKBENCH_AUTH_BEARER, "secret://api/oauth");
    assert(umi_web_workbench_auth_catalogue_upsert(&catalogue, &bearer) == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiWebWorkbenchAuthCatalogue, &catalogue,
        (umi_web_workbench_auth_catalogue_upsert(&catalogue, &bearer)));

    umi_web_workbench_request_init(&request, "r", "R", UMI_HTTP_METHOD_GET,
        "https://api.example.test/items");
    assert(umi_web_workbench_auth_apply(&bearer, "temporary-token", &request) == UMI_STATUS_OK);
    assert(strcmp(umi_web_workbench_request_header(&request, "Authorization"),
        "Bearer temporary-token") == 0);
    assert(strcmp(catalogue.profiles[0].secret_reference,
        "secret://api/oauth") == 0);
    umi_web_workbench_auth_profile_init(&query, "key", "API Key",
        UMI_WEB_WORKBENCH_AUTH_API_KEY_QUERY, "secret://api/key");
    assert(umi_web_workbench_copy_text(query.field_name, sizeof(query.field_name), "key") == UMI_STATUS_OK);
    assert(umi_web_workbench_auth_apply(&query, "a b", &request) == UMI_STATUS_OK);
    assert(strstr(request.url, "key=a%20b") != NULL);

    /* Check the real Basic header, including the final padded byte group.
     * Refusing an oversize secret must preserve the existing request. */
    UmiWebWorkbenchAuthProfile basic;
    umi_web_workbench_auth_profile_init(&basic, "basic", "Basic",
        UMI_WEB_WORKBENCH_AUTH_BASIC, "secret://api/basic");
    assert(umi_web_workbench_copy_text(basic.public_identity, sizeof(basic.public_identity), "foo") == UMI_STATUS_OK);
    assert(umi_web_workbench_auth_apply(&basic, "bar", &request) == UMI_STATUS_OK);
    assert(strcmp(umi_web_workbench_request_header(&request, "Authorization"), "Basic Zm9vOmJhcg==") == 0);
    char oversize[UMI_WEB_HEADER_VALUE_CAPACITY + 1U];
    memset(oversize, 'x', sizeof(oversize) - 1U);
    oversize[sizeof(oversize) - 1U] = '\0';
    assert(umi_web_workbench_auth_apply(&basic, oversize, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
    oversize[sizeof(oversize) - 9U] = '\0';
    assert(umi_web_workbench_auth_apply(&basic, oversize, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
    /* A complete encoding can still leave too little room for "Basic ". */
    const size_t rawCapacity = ((UMI_WEB_HEADER_VALUE_CAPACITY - 1U) / 4U) * 3U;
    oversize[rawCapacity - 4U] = '\0';
    assert(umi_web_workbench_auth_apply(&basic, oversize, &request) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(umi_web_workbench_request_header(&request, "Authorization"), "Basic Zm9vOmJhcg==") == 0);
    return 0;
}
