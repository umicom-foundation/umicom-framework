/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/security_catalogue.c
 *
 * PURPOSE:
 *   Implement the security catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Security governance capabilities v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/security_catalogue.h"
#include <stddef.h>
static const char *CAPABILITIES[] = {
    "umicom.security.identity-directory/2",
    "umicom.security.role-policy/2",
    "umicom.security.permission-catalogue/2",
    "umicom.security.workspace-trust-policy/2",
    "umicom.security.secret-inventory/2",
    "umicom.security.structured-redaction/2",
    "umicom.security.approval-workflow/2",
    "umicom.security.package-trust/2",
    "umicom.security.governance-audit/2",
    "umicom.security.policy-centre/2"
};
size_t umi_security_catalogue_count(void) { return sizeof(CAPABILITIES) / sizeof(CAPABILITIES[0]); }
const char *umi_security_catalogue_at(size_t index) { return index < umi_security_catalogue_count() ? CAPABILITIES[index] : NULL; }
