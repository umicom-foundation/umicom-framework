/* Umicom Framework Tests | Permission and role policy v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "security_governance_fixture.h"
int main(void)
{
    UmiSecurityGovernanceCentre centre;
    umi_test_security_governance(&centre);
    assert(umi_security_role_policy_allows(&centre.roles,"developer","project.build"));
    assert(!umi_security_role_policy_allows(&centre.roles,"developer","plugin.install"));
    assert(umi_security_role_assignment_find(&centre.assignments,"sammy","developer","workspace://demo",1U) != NULL);
    assert(umi_security_role_assignment_revoke(&centre.assignments,"assignment-sammy-developer") == UMI_STATUS_OK);
    assert(umi_security_role_assignment_find(&centre.assignments,"sammy","developer","workspace://demo",1U) == NULL);
    return 0;
}
