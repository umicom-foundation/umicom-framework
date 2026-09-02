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

/**
 * Represent the verification check data shared with callers of this public contract.
 */
typedef struct UmiVerificationCheck {
    char check_id[UMI_DELIVERY_ID_CAPACITY];
    UmiEvidenceStatus status;
} UmiVerificationCheck;
/**
 * Represent the verification report data shared with callers of this public contract.
 */
typedef struct UmiVerificationReport {
    UmiVerificationCheck checks[UMI_DELIVERY_MAX_CHECKS];
    size_t count;
} UmiVerificationReport;
/**
 * Initialise verification report from caller-provided values so later operations receive a
 * known state.
 */
void umi_verification_report_init(UmiVerificationReport *report);
/**
 * Add verification report only after its inputs and available capacity have been checked.
 */
UmiStatus umi_verification_report_add(UmiVerificationReport *report,
                                      const char *check_id,
                                      UmiEvidenceStatus status);
/**
 * Provide the verification report passed operation used by this module and its client
 * applications.
 */
int umi_verification_report_passed(const UmiVerificationReport *report);

#ifdef __cplusplus
}
#endif

#endif
