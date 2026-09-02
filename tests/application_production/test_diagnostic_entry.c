/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_diagnostic_entry.c
 *
 * PURPOSE:
 *   Implement the test diagnostic entry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | diagnostic_entry | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiApplicationProductionDiagnosticEntry entry;
    assert(umi_application_production_diagnostic_entry_set(&entry, UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_WARNING, "production.test", "org.umicom.studio", "Focused warning.") == UMI_STATUS_OK);
    assert(entry.severity == UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_WARNING);
    return 0;
}

