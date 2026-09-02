/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/change_governance.h
 *
 * PURPOSE:
 *   Record approval and effective-date governance for reporting-rule changes.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_CHANGE_GOVERNANCE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_CHANGE_GOVERNANCE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the change governance data shared with callers of this public contract.
 */
typedef struct UmiChangeGovernance {
    char change_id[UMI_REG_ID_CAPACITY];
    char approver_id[UMI_REG_ID_CAPACITY];
    uint64_t change_hash;
    int64_t effective_ms;
} UmiChangeGovernance;

/* Initialise one validated record approval and effective-date governance for reporting-rule changes. record. */
UmiStatus umi_reg_change_governance_init(UmiChangeGovernance *record, const char *change_id, const char *approver_id, uint64_t change_hash, int64_t effective_ms);

#ifdef __cplusplus
}
#endif

#endif
