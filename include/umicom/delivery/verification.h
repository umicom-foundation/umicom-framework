/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/verification.h
 *
 * PURPOSE:
 *   Aggregate named verification checks such as checksum, signature, SBOM and smoke tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Verification collects independent evidence before a release gate decides whether publishing is allowed.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_VERIFICATION_H
#define INCLUDE_UMICOM_DELIVERY_VERIFICATION_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVerificationCheck {
    char check_id[UMI_DELIVERY_ID_CAPACITY];
    UmiEvidenceStatus status;
} UmiVerificationCheck;
typedef struct UmiVerificationReport {
    UmiVerificationCheck checks[UMI_DELIVERY_MAX_CHECKS];
    size_t count;
} UmiVerificationReport;
void umi_verification_report_init(UmiVerificationReport *report);
UmiStatus umi_verification_report_add(UmiVerificationReport *report,
                                      const char *check_id,
                                      UmiEvidenceStatus status);
int umi_verification_report_passed(const UmiVerificationReport *report);

#ifdef __cplusplus
}
#endif

#endif
