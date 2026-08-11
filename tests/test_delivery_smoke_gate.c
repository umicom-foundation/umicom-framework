/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_smoke_gate.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/smoke_test.h"
#include "umicom/delivery/gate.h"
int main(void) {
    UmiSmokeReport report;
    UmiReleaseGate gate;
    umi_smoke_report_init(&report);
    assert(umi_smoke_report_add(&report, "launch", UMI_EVIDENCE_PASS, "ok") == UMI_STATUS_OK);
    assert(umi_smoke_report_passed(&report));
    assert(umi_release_gate_init(&gate, "smoke", 1) == UMI_STATUS_OK);
    assert(umi_release_gate_record(&gate, UMI_EVIDENCE_PASS, "launch ok") == UMI_STATUS_OK);
    assert(umi_release_gate_passed(&gate));
    return 0;
}
