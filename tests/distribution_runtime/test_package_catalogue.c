/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_package_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded package catalogue with name/version/target uniqueness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_catalogue.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrPackageCatalogue c; UmiDrComponentPackage item; umi_dr_package_catalogue_init(&c); umi_dr_component_package_init(&item); CHECK(umi_dr_copy_text(item.id, sizeof(item.id), "item") == UMI_STATUS_OK); CHECK(umi_dr_package_catalogue_add(&c,&item)==UMI_STATUS_OK); CHECK(umi_dr_package_catalogue_add(&c,&item)==UMI_STATUS_ALREADY_EXISTS); CHECK(umi_dr_package_catalogue_find(&c,"item")!=NULL);
    return 0;
}
