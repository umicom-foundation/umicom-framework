/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_ai_assisted_change.c
 *
 * PURPOSE:
 *   Execute and verify the studio.ai-assisted-change acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("studio.ai-assisted-change", "org.umicom.studio", "ai-assisted");
    return 0;
}

