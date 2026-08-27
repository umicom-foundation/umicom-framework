/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_visual_designer_round_trip.c
 *
 * PURPOSE:
 *   Execute and verify the studio.visual-designer-round-trip acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("studio.visual-designer-round-trip", "org.umicom.studio", "development");
    return 0;
}

