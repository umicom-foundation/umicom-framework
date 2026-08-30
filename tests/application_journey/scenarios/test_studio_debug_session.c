/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_debug_session.c
 *
 * PURPOSE:
 *   Execute and verify the studio.debug-session acceptance scenario.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("studio.debug-session", "org.umicom.studio", "development");
    return 0;
}

