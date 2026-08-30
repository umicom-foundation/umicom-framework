/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_resource_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for resource-pack identity, locale, scale and content metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/resource_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrResourceManifest value; umi_dr_resource_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"base")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.digest,sizeof(value.digest),"d")==UMI_STATUS_OK); CHECK(umi_dr_resource_manifest_valid(&value)); CHECK(umi_dr_resource_manifest_fingerprint(&value) != 0U);
    return 0;
}
