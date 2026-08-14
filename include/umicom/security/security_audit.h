/* Umicom Framework | Security governance audit v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_AUDIT_H
#define UMICOM_SECURITY_AUDIT_H
#include "umicom/security/governance.h"
typedef struct UmiSecurityAuditEntry {
    uint64_t sequence;
    uint64_t timestamp_ns;
    uint64_t correlation_id;
    char principal[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char action[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char resource[UMI_SECURITY_GOVERNANCE_PATH_CAPACITY];
    UmiSecurityAuditOutcome outcome;
    char reason[UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY];
} UmiSecurityAuditEntry;
typedef struct UmiSecurityAuditTrail { UmiSecurityAuditEntry items[UMI_SECURITY_GOVERNANCE_MAX_AUDIT]; size_t count; uint64_t next_sequence; } UmiSecurityAuditTrail;
UmiStatus umi_security_audit_append(UmiSecurityAuditTrail *trail,const UmiSecurityAuditEntry *entry);
const UmiSecurityAuditEntry *umi_security_audit_at(const UmiSecurityAuditTrail *trail,size_t index);
size_t umi_security_audit_denied_count(const UmiSecurityAuditTrail *trail);
#endif
