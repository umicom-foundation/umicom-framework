/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_status.c
 *
 * PURPOSE:
 *   Implement the test status behavior for
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
 * File: tests/application_experience/test_status.c
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/experience_status.h"

/*
 * Exercise test experience status and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_experience_status(void)
{
    UmiApplicationExperienceStatus status;
    const UmiApplicationExperienceDefinition *studio =
        umi_application_experience_catalogue_find("org.umicom.studio");
    assert(umi_application_experience_status(studio, &status) == UMI_STATUS_OK);
    assert(status.structure_size == sizeof(status));
    assert(status.feature_count == studio->feature_count);
    assert(status.readiness_percent <= 100U);
    assert(status.verified_count > 0U);
    return 0;
}
