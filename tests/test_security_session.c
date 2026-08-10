/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_session.c
 *
 * PURPOSE:
 *   Verify issuing, validating, expiring and revoking sessions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/security/session.h"
int main(void) {
    UmiSessionRegistry *r = NULL; UmiSessionRecord issued, validated;
    assert(umi_session_registry_create(&r) == UMI_STATUS_OK);
    assert(umi_session_registry_issue(r, "user.sammy", 100U, 1000U, &issued) == UMI_STATUS_OK);
    assert(umi_session_registry_validate(r, issued.session_id, 500U, &validated) == UMI_STATUS_OK);
    assert(umi_session_registry_revoke(r, issued.session_id) == UMI_STATUS_OK);
    assert(umi_session_registry_validate(r, issued.session_id, 600U, &validated) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_session_registry_cleanup(r, 2000U) == 1U);
    umi_session_registry_destroy(r); return 0;
}
