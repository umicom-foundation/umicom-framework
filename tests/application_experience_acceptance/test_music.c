/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience_acceptance/test_music.c
 *
 * PURPOSE:
 *   Verify the canonical music Framework experience remains valid with its established identity and default layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/application/experiences/experiences.h"

int test_application_experience_acceptance_music(void)
{
    const UmiApplicationExperienceDefinition *definition =
        umi_application_experience_music();

    assert(definition != NULL);
    assert(umi_application_experience_validate(
        definition) == UMI_STATUS_OK);
    assert(strcmp(
        definition->application_id,
        "org.umicom.music-studio") == 0);
    assert(definition->default_layout_id != NULL);
    assert(definition->default_layout_id[0] != '\0');
    assert(definition->panel_count > 0U);
    assert(definition->layout_count > 0U);
    assert(definition->feature_count > 0U);
    assert(umi_application_experience_layout_find(
        definition,
        definition->default_layout_id) != NULL);

    return 0;
}
