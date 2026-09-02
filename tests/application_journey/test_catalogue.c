/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/test_catalogue.c
 *
 * PURPOSE:
 *   Verify the bounded Studio and Trader journey catalogue and stable lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/journey/catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationJourneyCatalogue catalogue;
    assert(umi_application_journey_catalogue_build(&catalogue) == UMI_STATUS_OK);
    assert(catalogue.count == 16U);
    assert(umi_application_journey_catalogue_count_for(
               &catalogue, "org.umicom.studio") == 8U);
    assert(umi_application_journey_catalogue_count_for(
               &catalogue, "org.umicom.trader") == 8U);
    assert(umi_application_journey_catalogue_find(
               &catalogue, "trader.live-release-gate") != NULL);
    return 0;
}

