/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_portfolio_alignment.c
 *
 * PURPOSE:
 *   Prove every canonical product experience resolves to the authoritative
 *   Framework application portfolio.  This prevents suite/product metadata
 *   from drifting into application repositories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stddef.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/portfolio.h"

int test_experience_portfolio_alignment(void)
{
    size_t index;

    for (index = 0U; index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        const UmiApplicationDefinition *application;

        assert(experience != NULL);
        application = umi_application_portfolio_find(
            experience->application_id);
        assert(application != NULL);
        assert(application->application_id != NULL);
        assert(application->display_name != NULL);
        assert(application->repository_slug != NULL);
    }

    assert(umi_application_portfolio_find("org.umicom.bank") != NULL);
    assert(umi_application_portfolio_find("org.umicom.exchange") != NULL);
    assert(umi_application_portfolio_find("org.umicom.accountant") != NULL);
    assert(umi_application_portfolio_find("org.umicom.desktop") != NULL);
    return 0;
}
