/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_command_catalogue.c
 *
 * PURPOSE:
 *   Implement the test command catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | command_catalogue | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    assert(umi_application_production_control_command_count() >= 10U);
    assert(umi_application_production_control_command_find("application.production.checkpoint") != NULL);
    assert(umi_application_production_control_command_at(999U) == NULL);
    return 0;
}

