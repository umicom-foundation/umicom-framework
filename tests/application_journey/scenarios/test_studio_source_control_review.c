/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_studio_source_control_review.c
 *
 * PURPOSE:
 *   Execute and verify the studio.source-control-review acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("studio.source-control-review", "org.umicom.studio", "review");
    return 0;
}

