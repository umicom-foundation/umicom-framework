/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_identity.c
 *
 * PURPOSE:
 *   Verify identity registration, lookup and enablement changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "umicom/security/identity.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIdentityRegistry *r = NULL; UmiIdentityRecord in = {0}, out;
    (void)strcpy(in.identity_id, "user.sammy"); (void)strcpy(in.display_name, "Sammy Hegab"); in.kind = UMI_IDENTITY_HUMAN; in.enabled = 1;
    assert(umi_identity_registry_create(&r) == UMI_STATUS_OK);
    assert(umi_identity_registry_register(r, &in) == UMI_STATUS_OK);
    assert(umi_identity_registry_count(r) == 1U);
    assert(umi_identity_registry_get(r, "user.sammy", &out) == UMI_STATUS_OK && out.enabled);
    assert(umi_identity_registry_set_enabled(r, "user.sammy", 0) == UMI_STATUS_OK);
    assert(umi_identity_registry_get(r, "user.sammy", &out) == UMI_STATUS_OK && !out.enabled);
    umi_identity_registry_destroy(r); return 0;
}
