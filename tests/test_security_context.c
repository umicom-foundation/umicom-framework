/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_context.c
 *
 * PURPOSE:
 *   Verify the complete process security context creates every authoritative
 *   registry.
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
#include "umicom/security/context.h"
int main(void) {
    UmiSecurityContext *c = NULL;
    assert(umi_security_context_create(&c) == UMI_STATUS_OK);
    assert(umi_security_context_identities(c) != NULL);
    assert(umi_security_context_roles(c) != NULL);
    assert(umi_security_context_sessions(c) != NULL);
    assert(umi_security_context_secret_providers(c) != NULL);
    assert(umi_secret_provider_registry_count(umi_security_context_secret_providers(c)) == 1U);
    assert(umi_redactor_pattern_count(umi_security_context_redactor(c)) >= 6U);
    umi_security_context_destroy(c); return 0;
}
