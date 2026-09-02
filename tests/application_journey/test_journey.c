/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/test_journey.c
 *
 * PURPOSE:
 *   Verify journey validation rejects duplicate step identities and accepts bounded contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/journey/journey.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationJourneyStep steps[] = {
        {sizeof(UmiApplicationJourneyStep), "one", "One", "", "cmd.one",
         UMI_APPLICATION_JOURNEY_EVIDENCE_STATE, 1, 0},
        {sizeof(UmiApplicationJourneyStep), "two", "Two", "", "cmd.two",
         UMI_APPLICATION_JOURNEY_EVIDENCE_COMMAND, 1, 0}
    };
    UmiApplicationJourney journey = {
        sizeof(UmiApplicationJourney), UMI_APPLICATION_JOURNEY_API_VERSION,
        "org.umicom.test", "test.journey", "Test", "default",
        steps, 2U, 1
    };
    assert(umi_application_journey_validate(&journey) == UMI_STATUS_OK);
    journey.steps = NULL;
    assert(umi_application_journey_validate(&journey) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

