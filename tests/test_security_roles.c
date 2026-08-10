/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_roles.c
 *
 * PURPOSE:
 *   Verify role definitions, wildcard grants and permission revocation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/security/role.h"
int main(void) {
    UmiRoleRegistry *r = NULL; UmiRoleSnapshot role;
    assert(umi_role_registry_create(&r) == UMI_STATUS_OK);
    assert(umi_role_registry_define(r, "developer", "Umicom developer") == UMI_STATUS_OK);
    assert(umi_role_registry_grant(r, "developer", "studio.*") == UMI_STATUS_OK);
    assert(umi_role_registry_allows(r, "developer", "studio.build"));
    assert(!umi_role_registry_allows(r, "developer", "data.delete"));
    assert(umi_role_registry_at(r, 0U, &role) == UMI_STATUS_OK && role.permission_count == 1U);
    assert(umi_role_registry_revoke(r, "developer", "studio.*") == UMI_STATUS_OK);
    assert(!umi_role_registry_allows(r, "developer", "studio.build"));
    umi_role_registry_destroy(r); return 0;
}
