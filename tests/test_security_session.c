/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_session.c
 *
 * PURPOSE:
 *   Verify issuing, validating, expiring and revoking sessions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "umicom/security/session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiSessionRegistry *r = NULL; UmiSessionRecord issued, validated;
    assert(umi_session_registry_create(&r) == UMI_STATUS_OK);
    assert(umi_session_registry_issue(r, "user.sammy", 100U, 1000U, &issued) == UMI_STATUS_OK);
    assert(umi_session_registry_validate(r, issued.session_id, 500U, &validated) == UMI_STATUS_OK);
    assert(umi_session_registry_revoke(r, issued.session_id) == UMI_STATUS_OK);
    assert(umi_session_registry_validate(r, issued.session_id, 600U, &validated) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_session_registry_cleanup(r, 2000U) == 1U);
    UmiSessionRecord first, second, other;
    assert(umi_session_registry_issue(r, "learner", 2000U, 1000U, &first) == UMI_STATUS_OK);
    assert(umi_session_registry_issue(r, "learner", 2000U, 1000U, &second) == UMI_STATUS_OK);
    assert(umi_session_registry_issue(r, "learner-other", 2000U, 1000U, &other) == UMI_STATUS_OK);
    size_t revoked = 99U;
    assert(umi_session_registry_revoke_principal(r, "", &revoked) == UMI_STATUS_INVALID_ARGUMENT);
    assert(revoked == 99U);
    char unterminated[UMI_IDENTITY_ID_CAPACITY];
    memset(unterminated, 'x', sizeof(unterminated));
    assert(umi_session_registry_revoke_principal(r, unterminated, &revoked) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(revoked == 99U);
    assert(umi_session_registry_revoke_principal(r, "learner", &revoked) == UMI_STATUS_OK && revoked == 2U);
    assert(umi_session_registry_revoke_principal(r, "learner", &revoked) == UMI_STATUS_OK && revoked == 0U);
    assert(umi_session_registry_validate(r, first.session_id, 2050U, &validated) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_session_registry_validate(r, second.session_id, 2050U, &validated) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_session_registry_validate(r, other.session_id, 2050U, &validated) == UMI_STATUS_OK);
    assert(umi_session_registry_cleanup(r, 2050U) == 2U);
    assert(umi_session_registry_count(r) == 1U);
    assert(umi_session_registry_validate(r, other.session_id, 2050U, &validated) == UMI_STATUS_OK);
    umi_session_registry_destroy(r); return 0;
}
