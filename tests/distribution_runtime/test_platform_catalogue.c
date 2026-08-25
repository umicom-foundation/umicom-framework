/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_platform_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded registry of supported operating-system targets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/platform_catalogue.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrPlatformCatalogue c; UmiDrPlatformDescriptor item; umi_dr_platform_catalogue_init(&c); umi_dr_platform_descriptor_init(&item); CHECK(umi_dr_copy_text(item.id, sizeof(item.id), "item") == UMI_STATUS_OK); CHECK(umi_dr_platform_catalogue_add(&c,&item)==UMI_STATUS_OK); CHECK(umi_dr_platform_catalogue_add(&c,&item)==UMI_STATUS_ALREADY_EXISTS); CHECK(umi_dr_platform_catalogue_find(&c,"item")!=NULL);
    return 0;
}
