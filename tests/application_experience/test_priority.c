/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_priority.c
 *
 * PURPOSE:
 *   Implement the test priority behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_priority.c
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/experience_plan.h"

int test_experience_priority(void)
{
    const UmiApplicationExperienceDefinition *trader =
        umi_application_experience_catalogue_find("org.umicom.trader");
    const UmiExperienceFeatureDefinition *next =
        umi_application_experience_next_feature(trader);
    assert(next != NULL);
    assert(next->state != UMI_EXPERIENCE_FEATURE_VERIFIED);
    assert(umi_application_experience_priority_count(
        trader, UMI_EXPERIENCE_PRIORITY_P1) > 0U);
    return 0;
}
