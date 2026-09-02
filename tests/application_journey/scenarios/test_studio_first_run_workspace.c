/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_first_run_workspace.c
 *
 * PURPOSE:
 *   Execute and verify the studio.first-run-workspace acceptance scenario.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    verify_journey("studio.first-run-workspace", "org.umicom.studio", "development");
    return 0;
}

