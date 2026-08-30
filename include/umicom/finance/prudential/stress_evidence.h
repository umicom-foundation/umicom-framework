/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_evidence.h
 *
 * PURPOSE:
 *   Fingerprint immutable stress-test evidence by source and sequence.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_EVIDENCE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_EVIDENCE_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStressEvidence { uint64_t sequence; uint64_t fingerprint; char source[UMI_PRU_TEXT_CAPACITY]; } UmiPrudentialStressEvidence;
/* Initialise immutable stress evidence with deterministic source fingerprinting. */
UmiStatus umi_pru_stress_evidence_init(UmiPrudentialStressEvidence *evidence, uint64_t sequence, const char *source);

#ifdef __cplusplus
}
#endif

#endif
