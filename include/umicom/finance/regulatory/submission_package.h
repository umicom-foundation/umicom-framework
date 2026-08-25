/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/submission_package.h
 *
 * PURPOSE:
 *   Describe an immutable regulatory submission package fingerprint.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_PACKAGE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_SUBMISSION_PACKAGE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSubmissionPackage {
    char package_id[UMI_REG_ID_CAPACITY];
    char report_id[UMI_REG_ID_CAPACITY];
    char format[UMI_REG_NAME_CAPACITY];
    uint64_t fingerprint;
} UmiSubmissionPackage;

/* Initialise one validated describe an immutable regulatory submission package fingerprint. record. */
UmiStatus umi_reg_submission_package_init(UmiSubmissionPackage *record, const char *package_id, const char *report_id, const char *format, uint64_t fingerprint);

#ifdef __cplusplus
}
#endif

#endif
