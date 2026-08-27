/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_catalogue.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/build_readiness/catalogue.h"
int main(void) {
    size_t index;
    assert(umi_test_platform_build_artifact_catalogue_count() == 63U);
    assert(umi_test_platform_product_validation_profile_count() == 24U);
    assert(umi_test_platform_build_artifact_catalogue_find_test(
        "framework.ui_workstation.trader.layouts") != NULL);
    assert(umi_test_platform_build_artifact_catalogue_find_test(
        "studio.editor_intelligence_workbench_contribution") != NULL);
    assert(strcmp(umi_test_platform_product_validation_profile_find(
        "trader")->preset, "windows-ucrt64-all-debug") == 0);
    for (index = 0U; index < umi_test_platform_build_artifact_catalogue_count();
         ++index) {
        assert(umi_test_platform_build_artifact_validate(
            umi_test_platform_build_artifact_catalogue_at(index)) ==
            UMI_STATUS_OK);
    }
    return 0;
}

