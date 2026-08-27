/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_layout_recovery.c
 *
 * PURPOSE:
 *   Execute and verify the studio.layout-recovery acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("studio.layout-recovery", "org.umicom.studio", "review");
    return 0;
}

