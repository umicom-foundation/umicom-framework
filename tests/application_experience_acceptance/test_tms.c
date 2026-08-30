/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience_acceptance/test_tms.c
 *
 * PURPOSE:
 *   Verify the canonical tms Framework experience remains valid with its established identity and default layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/application/experiences/experiences.h"

int test_application_experience_acceptance_tms(void)
{
    const UmiApplicationExperienceDefinition *definition =
        umi_application_experience_tms();

    assert(definition != NULL);
    assert(umi_application_experience_validate(
        definition) == UMI_STATUS_OK);
    assert(strcmp(
        definition->application_id,
        "org.umicom.tms") == 0);
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
