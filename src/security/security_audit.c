/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/security_audit.c
 *
 * PURPOSE:
 *   Implement the security audit behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Security governance audit v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/security_audit.h"
#include <string.h>
/* Add security audit only after its inputs and available capacity have been checked. */
UmiStatus umi_security_audit_append(UmiSecurityAuditTrail *trail,const UmiSecurityAuditEntry *entry)
{
    UmiSecurityAuditEntry copy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (trail == NULL || entry == NULL || entry->principal[0] == '\0' || entry->action[0] == '\0' || entry->resource[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    copy = *entry;
    trail->next_sequence += 1U;
    copy.sequence = trail->next_sequence;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (trail->count < UMI_SECURITY_GOVERNANCE_MAX_AUDIT) trail->items[trail->count++] = copy;
    /* Use this fallback path when the earlier condition does not apply. */
    else { (void)memmove(&trail->items[0],&trail->items[1],(UMI_SECURITY_GOVERNANCE_MAX_AUDIT - 1U) * sizeof(trail->items[0])); trail->items[UMI_SECURITY_GOVERNANCE_MAX_AUDIT - 1U] = copy; }
    return UMI_STATUS_OK;
}
/*
 * Find security audit while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSecurityAuditEntry *umi_security_audit_at(const UmiSecurityAuditTrail *trail,size_t index) { return trail != NULL && index < trail->count ? &trail->items[index] : NULL; }
/*
 * Return the number of records represented by security audit denied without changing their
 * state.
 */
size_t umi_security_audit_denied_count(const UmiSecurityAuditTrail *trail)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (trail == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < trail->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (trail->items[index].outcome == UMI_SECURITY_AUDIT_DENIED) count += 1U;
    return count;
}
