/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_edit_build_test.c
 *
 * PURPOSE:
 *   Execute and verify the studio.edit-build-test acceptance scenario.
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
    verify_journey("studio.edit-build-test", "org.umicom.studio", "development");
    return 0;
}

