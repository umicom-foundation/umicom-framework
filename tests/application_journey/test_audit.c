/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/test_audit.c
 *
 * PURPOSE:
 *   Verify all journey references resolve through canonical experience definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/journey/audit.h"
int main(void)
{
    UmiApplicationJourneyCatalogue catalogue;
    UmiApplicationJourneyAudit audit;
    assert(umi_application_journey_catalogue_build(&catalogue) == UMI_STATUS_OK);
    assert(umi_application_journey_audit_catalogue(&catalogue, &audit) ==
           UMI_STATUS_OK);
    assert(audit.valid);
    assert(audit.journey_count == 16U);
    assert(audit.step_count == 80U);
    assert(audit.blocking_journey_count == 16U);
    assert(audit.critical_step_count == 80U);
    assert(audit.confirmation_step_count >= 10U);
    return 0;
}

