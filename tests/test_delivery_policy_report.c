/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_policy_report.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/policy.h"
#include "umicom/delivery/report.h"
int main(void) {
    UmiDeliveryPolicy policy = umi_delivery_policy_default();
    UmiBuildEvidence evidence;
    UmiDeliveryReport report;
    char text[256];
    umi_build_evidence_init(&evidence);
    evidence.build_succeeded = 1;
    evidence.tests_total = 1U;
    evidence.tests_passed = 1U;
    assert(umi_delivery_policy_check_build(&policy, &evidence) == UMI_STATUS_OK);
    umi_delivery_report_init(&report);
    report.ready = 1;
    assert(umi_delivery_report_format(&report, text, sizeof(text)) == UMI_STATUS_OK);
    return 0;
}
