/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_application_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for cross-platform application identity and runtime requirement manifest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrApplicationManifest value; umi_dr_application_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"studio") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.name,sizeof(value.name),"Umicom Studio") == UMI_STATUS_OK); CHECK(umi_dr_application_manifest_valid(&value)); CHECK(umi_dr_application_manifest_fingerprint(&value) != 0U);
    return 0;
}
