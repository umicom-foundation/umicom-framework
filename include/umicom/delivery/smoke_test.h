/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/smoke_test.h
 *
 * PURPOSE:
 *   Record post-install smoke checks used to prove a staged or installed application starts correctly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Smoke tests are deliberately small checks such as launching an executable or asking a health endpoint for a response.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_SMOKE_TEST_H
#define INCLUDE_UMICOM_DELIVERY_SMOKE_TEST_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSmokeCheck {
    char check_id[UMI_DELIVERY_ID_CAPACITY];
    UmiEvidenceStatus status;
    char message[UMI_DELIVERY_TEXT_CAPACITY];
} UmiSmokeCheck;

typedef struct UmiSmokeReport {
    UmiSmokeCheck checks[UMI_DELIVERY_MAX_CHECKS];
    size_t count;
} UmiSmokeReport;

void umi_smoke_report_init(UmiSmokeReport *report);
UmiStatus umi_smoke_report_add(UmiSmokeReport *report,
                               const char *check_id,
                               UmiEvidenceStatus status,
                               const char *message);
int umi_smoke_report_passed(const UmiSmokeReport *report);

#ifdef __cplusplus
}
#endif

#endif
