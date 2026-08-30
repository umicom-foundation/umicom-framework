/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_experience_portfolio_header.c
 *
 * PURPOSE:
 *   Verify the public experience portfolio header exposes both catalogue
 *   families through one unambiguous include for applications and tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/experience_portfolio.h"

int main(void)
{
    assert(umi_application_experience_catalogue_count() > 0U);
    assert(umi_application_experience_profile_catalogue_count() > 0U);
    return 0;
}
