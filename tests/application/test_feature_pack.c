/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_feature_pack.c
 *
 * PURPOSE:
 *   Verify every built-in feature pack refers only to canonical Framework
 *   capabilities and component catalogue entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/feature_pack.h"

int main(void)
{
    size_t index;

    assert(umi_application_feature_pack_catalogue_count() >= 8U);

    for (index = 0U;
         index < umi_application_feature_pack_catalogue_count();
         ++index) {
        const UmiApplicationFeaturePackDefinition *pack =
            umi_application_feature_pack_catalogue_at(index);

        assert(pack != NULL);
        assert(umi_application_feature_pack_validate(pack) == UMI_STATUS_OK);
        assert(umi_application_feature_pack_catalogue_find(pack->pack_id) ==
               pack);
    }

    assert(umi_application_feature_pack_catalogue_find(
        "umicom.pack.core-runtime") != NULL);
    assert(umi_application_feature_pack_catalogue_find(
        "umicom.pack.developer") != NULL);
    assert(umi_application_feature_pack_catalogue_find(
        "umicom.pack.ai-assistant") != NULL);

    return 0;
}
