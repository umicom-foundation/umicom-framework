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

int test_experience_catalogue(void)
{
    const UmiApplicationExperienceDefinition *studio;
    const UmiApplicationExperienceDefinition *bank;
    const UmiApplicationExperienceDefinition *music;
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
    return 0;
}
