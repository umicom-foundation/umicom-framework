/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_catalogue.c
 *
 * PURPOSE:
 *   Implement the test catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/runtime/readiness.h"

/*
 * Exercise test experience catalogue and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_experience_catalogue(void)
{
    const UmiApplicationExperienceDefinition *studio;
    const UmiApplicationExperienceDefinition *bank;
    const UmiApplicationExperienceDefinition *music;
    UmiApplicationLaunchReadinessSummary summary;
    size_t index;
    assert(umi_application_experience_catalogue_validate() == UMI_STATUS_OK);
    assert(umi_application_experience_catalogue_count() >= 20U);
    studio = umi_application_experience_catalogue_find("org.umicom.studio");
    bank = umi_application_experience_catalogue_find("org.umicom.bank");
    music = umi_application_experience_catalogue_find("org.umicom.music");
    assert(studio != NULL);
    assert(bank != NULL);
    /* Historical IDs resolve to the same immutable canonical experience. */
    assert(music == umi_application_experience_catalogue_find(
                        "org.umicom.music-studio"));
    assert(studio->layout_count >= 3U);
    assert(bank->panel_count >= 8U);
    /* Every published product must have a valid Framework default surface. */
    for (index = 0U; index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        UmiApplicationLaunchReadiness readiness;
        assert(experience != NULL);
        assert(umi_application_launch_readiness_check(
                   experience->application_id, &readiness) == UMI_STATUS_OK);
        assert(readiness.state == UMI_APPLICATION_LAUNCH_READINESS_READY);
        assert(readiness.launchable);
        assert(readiness.layout_count == experience->layout_count);
        assert(readiness.panel_count == experience->panel_count);
        assert(readiness.reason[0] != '\0');
    }
    /* Unknown IDs remain diagnosable without being offered as launch choices. */
    {
        UmiApplicationLaunchReadiness readiness;
        assert(umi_application_launch_readiness_check(
                   "org.umicom.unknown", &readiness) == UMI_STATUS_OK);
        assert(readiness.state ==
               UMI_APPLICATION_LAUNCH_READINESS_MISSING_EXPERIENCE);
        assert(!readiness.launchable);
    }
    /* The portfolio summary gives launchers one consistent overview of every product. */
    assert(umi_application_launch_readiness_summary(&summary) == UMI_STATUS_OK);
    assert(summary.structure_size == sizeof(summary));
    assert(summary.application_count == umi_application_experience_catalogue_count());
    assert(summary.ready_count == summary.application_count);
    assert(summary.blocked_count == 0U);
    assert(summary.average_feature_readiness_percent <= 100U);
    assert(summary.first_blocked_application_id[0] == '\0');
    assert(summary.first_blocked_reason[0] == '\0');
    /* A null destination is rejected before the summary attempts any writes. */
    assert(umi_application_launch_readiness_summary(NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
