/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_profiles.c
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stddef.h>
#include "umicom/application/experience_catalogue.h"

int test_experience_profiles(void)
{
    size_t index;
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
