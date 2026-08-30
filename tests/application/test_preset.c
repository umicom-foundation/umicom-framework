/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_preset.c
 *
 * PURPOSE:
 *   Verify reusable application archetypes resolve only registered feature packs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/preset.h"

int main(void)
{
    size_t index;

    assert(umi_application_preset_catalogue_count() >= 6U);

    for (index = 0U;
         index < umi_application_preset_catalogue_count();
         ++index) {
        const UmiApplicationPresetDefinition *preset =
            umi_application_preset_catalogue_at(index);

        assert(preset != NULL);
        assert(umi_application_preset_validate(preset) == UMI_STATUS_OK);
        assert(umi_application_preset_catalogue_find(preset->preset_id) ==
               preset);
    }

    assert(umi_application_preset_catalogue_find(
        "umicom.preset.developer-workbench") != NULL);
    assert(umi_application_preset_catalogue_find(
        "umicom.preset.web-service") != NULL);

    return 0;
}
