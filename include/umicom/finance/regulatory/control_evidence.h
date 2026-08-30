/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/control_evidence.h
 *
 * PURPOSE:
 *   Attach immutable evidence to a financial-control execution.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_EVIDENCE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CONTROL_EVIDENCE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiControlEvidence {
    char evidence_id[UMI_REG_ID_CAPACITY];
    char execution_id[UMI_REG_ID_CAPACITY];
    char locator[UMI_REG_TEXT_CAPACITY];
    uint64_t content_hash;
} UmiControlEvidence;

/* Initialise one validated attach immutable evidence to a financial-control execution. record. */
UmiStatus umi_reg_control_evidence_init(UmiControlEvidence *record, const char *evidence_id, const char *execution_id, const char *locator, uint64_t content_hash);

#ifdef __cplusplus
}
#endif

#endif
