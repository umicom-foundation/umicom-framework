/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_authorisation.c
 *
 * PURPOSE:
 *   Verify deny-by-default authorisation, explicit policy and role grants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/security/authorisation.h"
int main(void) {
    UmiPolicyEngine *p = NULL; UmiRoleRegistry *roles = NULL; UmiAuthorisationService *a = NULL; UmiAuthorisationDecision d;
    UmiPolicyRule rule = {"service.builder", "process.execute", "workspace/*", UMI_POLICY_ALLOW};
    assert(umi_policy_engine_create(&p) == UMI_STATUS_OK);
    assert(umi_role_registry_create(&roles) == UMI_STATUS_OK);
    assert(umi_role_registry_define(roles, "developer", "Developer") == UMI_STATUS_OK);
    assert(umi_role_registry_grant(roles, "developer", "studio.*") == UMI_STATUS_OK);
    assert(umi_authorisation_service_create(p, roles, &a) == UMI_STATUS_OK);
    assert(umi_authorisation_check(a, "user.sammy", "developer", "studio.build", "workspace/project", &d) == UMI_STATUS_OK && d.allowed);
    assert(umi_policy_engine_add(p, &rule) == UMI_STATUS_OK);
    assert(umi_authorisation_check(a, "service.builder", NULL, "process.execute", "workspace/project", &d) == UMI_STATUS_OK && d.allowed);
    assert(umi_authorisation_check(a, "guest", NULL, "data.delete", "records", &d) == UMI_STATUS_OK && !d.allowed);
    umi_authorisation_service_destroy(a); umi_role_registry_destroy(roles); umi_policy_engine_destroy(p); return 0;
}
