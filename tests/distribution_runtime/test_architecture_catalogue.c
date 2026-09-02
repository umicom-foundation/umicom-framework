/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_architecture_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded architecture target registry and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/architecture_catalogue.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrArchitectureCatalogue c; UmiDrArchitectureDescriptor item; umi_dr_architecture_catalogue_init(&c); umi_dr_architecture_descriptor_init(&item); CHECK(umi_dr_copy_text(item.id, sizeof(item.id), "item") == UMI_STATUS_OK); CHECK(umi_dr_architecture_catalogue_add(&c,&item)==UMI_STATUS_OK); CHECK(umi_dr_architecture_catalogue_add(&c,&item)==UMI_STATUS_ALREADY_EXISTS); CHECK(umi_dr_architecture_catalogue_find(&c,"item")!=NULL);
    return 0;
}
