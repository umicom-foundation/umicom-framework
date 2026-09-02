/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_evidence_record.c
 *
 * PURPOSE:
 *   Implement the test evidence record behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | evidence_record | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiApplicationProductionEvidenceRecord record;
    assert(umi_application_production_evidence_record_set(&record, "org.umicom.studio:tests", UMI_APPLICATION_PRODUCTION_EVIDENCE_TEST, UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED, "ctest:studio", 1U) == UMI_STATUS_OK);
    assert(umi_application_production_evidence_record_validate(&record) == UMI_STATUS_OK);
    return 0;
}

