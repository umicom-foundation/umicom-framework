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

/* Assert the universal application contract shared by every Framework-owned
 * experience without duplicating the portfolio or experience catalogues. */
static void assert_universal_application_contract(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationDefinition *application)
{
    const UmiExperienceLayoutDefinition *default_layout;

    assert(experience != NULL);
    assert(application != NULL);
    assert(umi_application_experience_validate(experience) == UMI_STATUS_OK);
    assert(umi_application_definition_validate(application) == UMI_STATUS_OK);
    assert(application->application_id != NULL);
    assert(application->display_name != NULL);
    assert(application->repository_slug != NULL);
    assert(application->executable_name != NULL);
    assert(application->workspace_profiles != NULL);
    assert(application->workspace_profile_count > 0U);
    assert(umi_application_definition_declares_capability(
        application, "umicom.ui"));
    assert(umi_application_definition_declares_capability(
        application, "umicom.application.federation"));
    assert((application->flags & UMI_APPLICATION_STANDALONE) != 0U);
    assert((application->flags & UMI_APPLICATION_FEDERATED) != 0U);
    assert((application->flags & UMI_APPLICATION_MULTI_WINDOW) != 0U);
    assert((application->flags & UMI_APPLICATION_MULTI_MONITOR) != 0U);

    assert(experience->default_layout_id != NULL);
    assert(experience->panels != NULL);
    assert(experience->panel_count > 0U);
    assert(experience->layouts != NULL);
    assert(experience->layout_count > 0U);
    assert(experience->features != NULL);
    assert(experience->feature_count > 0U);

    default_layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    assert(default_layout != NULL);
    assert(default_layout->panel_ids != NULL);
    assert(default_layout->panel_count > 0U);
}

/*
 * Exercise test experience portfolio alignment and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_experience_portfolio_alignment(void)
{
    size_t index;

    assert(umi_application_experience_catalogue_validate() == UMI_STATUS_OK);
    assert(umi_application_portfolio_count() >=
           umi_application_experience_catalogue_count());

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        const UmiApplicationDefinition *application;

        assert(experience != NULL);
        application = umi_application_portfolio_find(
            experience->application_id);
        assert_universal_application_contract(experience, application);
    }

    assert(umi_application_portfolio_find("org.umicom.bank") != NULL);
    assert(umi_application_portfolio_find("org.umicom.exchange") != NULL);
    assert(umi_application_portfolio_find("org.umicom.accountant") != NULL);
    assert(umi_application_portfolio_find("org.umicom.desktop") != NULL);
    return 0;
}
