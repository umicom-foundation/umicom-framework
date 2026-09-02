/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_profiles.c
 *
 * PURPOSE:
 *   Implement the test profiles behavior for
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
 * File: tests/application_experience/test_profiles.c
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stddef.h>
#include "umicom/application/experience_catalogue.h"

/*
 * Exercise test experience profiles and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_experience_profiles(void)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *definition =
            umi_application_experience_catalogue_at(index);
        assert(definition != NULL);
        assert(umi_application_experience_validate(definition) ==
               UMI_STATUS_OK);
        assert(umi_application_experience_layout_find(
            definition, definition->default_layout_id) != NULL);
    }
    return 0;
}
