/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_evidence.h
 *
 * PURPOSE:
 *   Represent immutable source evidence supporting a report fact.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_EVIDENCE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_EVIDENCE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportingEvidence {
    char evidence_id[UMI_REG_ID_CAPACITY];
    char source_id[UMI_REG_ID_CAPACITY];
    char locator[UMI_REG_TEXT_CAPACITY];
    uint64_t content_hash;
} UmiReportingEvidence;

/* Initialise one validated represent immutable source evidence supporting a report fact. record. */
UmiStatus umi_reg_reporting_evidence_init(UmiReportingEvidence *record, const char *evidence_id, const char *source_id, const char *locator, uint64_t content_hash);

#ifdef __cplusplus
}
#endif

#endif
