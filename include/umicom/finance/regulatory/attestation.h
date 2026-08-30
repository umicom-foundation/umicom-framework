/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/attestation.h
 *
 * PURPOSE:
 *   Record management attestation and signed-evidence reference.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_ATTESTATION_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_ATTESTATION_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRegAttestation {
    char attestation_id[UMI_REG_ID_CAPACITY];
    char report_id[UMI_REG_ID_CAPACITY];
    char signer_id[UMI_REG_ID_CAPACITY];
    uint64_t evidence_hash;
} UmiRegAttestation;

/* Initialise one validated record management attestation and signed-evidence reference. record. */
UmiStatus umi_reg_attestation_init(UmiRegAttestation *record, const char *attestation_id, const char *report_id, const char *signer_id, uint64_t evidence_hash);

#ifdef __cplusplus
}
#endif

#endif
