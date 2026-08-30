/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_audit.h
 *
 * PURPOSE:
 *   Maintain an ordered stress-testing audit sequence.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_AUDIT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_AUDIT_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStressAuditEntry { uint64_t sequence; uint64_t fingerprint; } UmiPrudentialStressAuditEntry;
typedef struct UmiPrudentialStressAudit { UmiPrudentialStressAuditEntry entries[UMI_PRU_MAX_ITEMS]; size_t count; } UmiPrudentialStressAudit;
/* Append an audit evidence fingerprint with strictly increasing sequence. */
UmiStatus umi_pru_stress_audit_append(UmiPrudentialStressAudit *audit, uint64_t sequence, uint64_t fingerprint);

#ifdef __cplusplus
}
#endif

#endif
