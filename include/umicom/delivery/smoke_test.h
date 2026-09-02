/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/smoke_test.h
 *
 * PURPOSE:
 *   Record post-install smoke checks used to prove a staged or installed application starts correctly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Smoke tests are deliberately small checks such as launching an executable or asking a health endpoint for a response.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_SMOKE_TEST_H
#define INCLUDE_UMICOM_DELIVERY_SMOKE_TEST_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the smoke check data shared with callers of this public contract.
 */
typedef struct UmiSmokeCheck {
    char check_id[UMI_DELIVERY_ID_CAPACITY];
    UmiEvidenceStatus status;
    char message[UMI_DELIVERY_TEXT_CAPACITY];
} UmiSmokeCheck;

/**
 * Represent the smoke report data shared with callers of this public contract.
 */
typedef struct UmiSmokeReport {
    UmiSmokeCheck checks[UMI_DELIVERY_MAX_CHECKS];
    size_t count;
} UmiSmokeReport;

/**
 * Initialise smoke report from caller-provided values so later operations receive a known
 * state.
 */
void umi_smoke_report_init(UmiSmokeReport *report);
/**
 * Add smoke report only after its inputs and available capacity have been checked.
 */
UmiStatus umi_smoke_report_add(UmiSmokeReport *report,
                               const char *check_id,
                               UmiEvidenceStatus status,
                               const char *message);
/**
 * Provide the smoke report passed operation used by this module and its client
 * applications.
 */
int umi_smoke_report_passed(const UmiSmokeReport *report);

#ifdef __cplusplus
}
#endif

#endif
