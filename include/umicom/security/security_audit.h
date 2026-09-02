/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/security_audit.h
 *
 * PURPOSE:
 *   Publish the public security audit contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_AUDIT_H
#define UMICOM_SECURITY_AUDIT_H
#include "umicom/security/governance.h"
/**
 * Represent the security audit entry data shared with callers of this public contract.
 */
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
/**
 * Represent the security audit trail data shared with callers of this public contract.
 */
typedef struct UmiSecurityAuditTrail { UmiSecurityAuditEntry items[UMI_SECURITY_GOVERNANCE_MAX_AUDIT]; size_t count; uint64_t next_sequence; } UmiSecurityAuditTrail;
/**
 * Add security audit only after its inputs and available capacity have been checked.
 */
UmiStatus umi_security_audit_append(UmiSecurityAuditTrail *trail,const UmiSecurityAuditEntry *entry);
/**
 * Find security audit while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSecurityAuditEntry *umi_security_audit_at(const UmiSecurityAuditTrail *trail,size_t index);
/**
 * Return the number of records represented by security audit denied without changing their
 * state.
 */
size_t umi_security_audit_denied_count(const UmiSecurityAuditTrail *trail);
#endif
