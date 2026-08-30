/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_bundle_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded bundle inventory with duplicate rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_catalogue.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrBundleCatalogue c; UmiDrApplicationBundle item; umi_dr_bundle_catalogue_init(&c); umi_dr_application_bundle_init(&item); CHECK(umi_dr_copy_text(item.id, sizeof(item.id), "item") == UMI_STATUS_OK); CHECK(umi_dr_bundle_catalogue_add(&c,&item)==UMI_STATUS_OK); CHECK(umi_dr_bundle_catalogue_add(&c,&item)==UMI_STATUS_ALREADY_EXISTS); CHECK(umi_dr_bundle_catalogue_find(&c,"item")!=NULL);
    return 0;
}
