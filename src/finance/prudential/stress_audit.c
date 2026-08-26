/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_audit.c
 *
 * PURPOSE:
 *   Maintain an ordered stress-testing audit sequence.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_audit.h"

UmiStatus umi_pru_stress_audit_append(UmiPrudentialStressAudit *audit,uint64_t sequence,uint64_t fingerprint) { if(audit==NULL||sequence==0U||fingerprint==0U)return UMI_STATUS_INVALID_ARGUMENT; if(audit->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; if(audit->count>0U&&sequence<=audit->entries[audit->count-1U].sequence)return UMI_STATUS_INVALID_STATE; audit->entries[audit->count].sequence=sequence; audit->entries[audit->count].fingerprint=fingerprint; ++audit->count; return UMI_STATUS_OK; }
