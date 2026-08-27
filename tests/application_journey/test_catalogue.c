/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/test_catalogue.c
 *
 * PURPOSE:
 *   Verify the bounded Studio and Trader journey catalogue and stable lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/journey/catalogue.h"
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

