/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_portfolio_coverage.c
 *
 * PURPOSE:
 *   Prove every application in the canonical portfolio has three projectable
 *   audience-aware windows and at least one visible panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/presentation.h"

#include <assert.h>

#include "umicom/application/portfolio.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t application_index;

    assert(umi_application_portfolio_count() == 26U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (application_index = 0U;
         application_index < umi_application_portfolio_count();
         ++application_index) {
        const UmiApplicationDefinition *application =
            umi_application_portfolio_at(application_index);
        size_t window_index;
        assert(application != NULL);
        assert(umi_application_presentation_window_catalogue_application_count(
                   application->application_id) == 3U);
        /* Visit each bounded item once so every record receives the same rule. */
        for (window_index = 0U; window_index < 3U; ++window_index) {
            const UmiApplicationPresentationWindowSpec *window =
                umi_application_presentation_window_catalogue_application_at(
                    application->application_id, window_index);
            UmiApplicationPresentationPlan plan;
            assert(window != NULL);
            assert(umi_application_presentation_project(
                       window->recipe_id, &plan) == UMI_STATUS_OK);
            assert(plan.placement_count > 0U);
            assert(plan.visible_count > 0U);
        }
    }
    return 0;
}
