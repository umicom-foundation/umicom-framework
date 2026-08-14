/* Umicom Framework | Security governance audit v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/security_audit_v2.h"
#include <string.h>
UmiStatus umi_security_audit_append(UmiSecurityAuditTrail *trail,const UmiSecurityAuditEntry *entry)
{
    UmiSecurityAuditEntry copy;
    if (trail == NULL || entry == NULL || entry->principal[0] == '\0' || entry->action[0] == '\0' || entry->resource[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    copy = *entry;
    trail->next_sequence += 1U;
    copy.sequence = trail->next_sequence;
    if (trail->count < UMI_SECURITY_GOVERNANCE_MAX_AUDIT) trail->items[trail->count++] = copy;
    else { (void)memmove(&trail->items[0],&trail->items[1],(UMI_SECURITY_GOVERNANCE_MAX_AUDIT - 1U) * sizeof(trail->items[0])); trail->items[UMI_SECURITY_GOVERNANCE_MAX_AUDIT - 1U] = copy; }
    return UMI_STATUS_OK;
}
const UmiSecurityAuditEntry *umi_security_audit_at(const UmiSecurityAuditTrail *trail,size_t index) { return trail != NULL && index < trail->count ? &trail->items[index] : NULL; }
size_t umi_security_audit_denied_count(const UmiSecurityAuditTrail *trail)
{
    size_t index;
    size_t count = 0U;
    if (trail == NULL) return 0U;
    for (index = 0U; index < trail->count; ++index) if (trail->items[index].outcome == UMI_SECURITY_AUDIT_DENIED) count += 1U;
    return count;
}
