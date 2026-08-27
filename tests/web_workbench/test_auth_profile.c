/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_auth_profile.c
 * PURPOSE: Verify secret-reference profiles and transient authentication use.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/auth_profile.h"

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
    return 0;
}
