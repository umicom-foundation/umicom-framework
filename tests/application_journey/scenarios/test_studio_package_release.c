/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_package_release.c
 *
 * PURPOSE:
 *   Execute and verify the studio.package-release acceptance scenario.
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
    verify_journey("studio.package-release", "org.umicom.studio", "review");
    return 0;
}

