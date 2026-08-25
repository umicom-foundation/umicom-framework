/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_install_registry.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded registry of installed applications and versions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/install_registry.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrInstallRegistry c; UmiDrInstallState item; umi_dr_install_registry_init(&c); umi_dr_install_state_init(&item); CHECK(umi_dr_copy_text(item.id, sizeof(item.id), "item") == UMI_STATUS_OK); CHECK(umi_dr_install_registry_add(&c,&item)==UMI_STATUS_OK); CHECK(umi_dr_install_registry_add(&c,&item)==UMI_STATUS_ALREADY_EXISTS); CHECK(umi_dr_install_registry_find(&c,"item")!=NULL);
    return 0;
}
