/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_edit_build_test.c
 *
 * PURPOSE:
 *   Execute and verify the studio.edit-build-test acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("studio.edit-build-test", "org.umicom.studio", "development");
    return 0;
}

