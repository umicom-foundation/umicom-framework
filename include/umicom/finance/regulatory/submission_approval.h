/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/submission_approval.h
 *
 * PURPOSE:
 *   Record maker-checker approval for one submission package.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_APPROVAL_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_APPROVAL_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSubmissionApproval {
    char package_id[UMI_REG_ID_CAPACITY];
    char approver_id[UMI_REG_ID_CAPACITY];
    int64_t approved_ms;
    int approved;
} UmiSubmissionApproval;

/* Initialise one validated record maker-checker approval for one submission package. record. */
UmiStatus umi_reg_submission_approval_init(UmiSubmissionApproval *record, const char *package_id, const char *approver_id, int64_t approved_ms, int approved);

#ifdef __cplusplus
}
#endif

#endif
