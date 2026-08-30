/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_sdk_export_catalogue_v2.c
 *
 * PURPOSE:
 *   Verify installed SDK target catalogues and duplicate protection.
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
#include "umicom/sdk/sdk.h"

int main(void)
{
    UmiSdkExportCatalogueV2 catalogue;
    const UmiSdkExportedTargetV2 *target;
    umi_sdk_export_catalogue_v2_init(&catalogue);
    assert(umi_sdk_export_catalogue_v2_add(
               &catalogue, "base", "Umicom::base", 1) == UMI_STATUS_OK);
    assert(umi_sdk_export_catalogue_v2_add(
               &catalogue, "delivery", "Umicom::delivery", 1) == UMI_STATUS_OK);
    assert(umi_sdk_export_catalogue_v2_add(
               &catalogue, "base", "Umicom::base", 1) ==
           UMI_STATUS_ALREADY_EXISTS);
    target = umi_sdk_export_catalogue_v2_find(&catalogue, "delivery");
    assert(target != NULL);
    assert(strcmp(target->target_name, "Umicom::delivery") == 0);
    assert(umi_sdk_export_catalogue_v2_ready(&catalogue));
    return 0;
}
