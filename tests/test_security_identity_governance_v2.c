/* Umicom Framework Tests | Identity governance v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "security_governance_fixture.h"
int main(void)
{
    UmiSecurityGovernanceCentre centre;
    const UmiSecurityIdentityProfile *identity;
    umi_test_security_governance(&centre);
    identity = umi_security_identity_directory_find(&centre.identities,"sammy");
    assert(identity != NULL && identity->enabled && identity->assurance_level == 2U);
    assert(umi_security_identity_directory_set_enabled(&centre.identities,"sammy",false) == UMI_STATUS_OK);
    assert(!umi_security_identity_directory_find(&centre.identities,"sammy")->enabled);
    return 0;
}
