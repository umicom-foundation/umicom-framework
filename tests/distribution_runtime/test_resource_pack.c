/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_resource_pack.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded resource-pack entries and aggregate size accounting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/resource_pack.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrResourcePack p; UmiDrResourceManifest i; umi_dr_resource_pack_init(&p); umi_dr_resource_manifest_init(&i); CHECK(umi_dr_copy_text(i.id,sizeof(i.id),"r")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(i.digest,sizeof(i.digest),"d")==UMI_STATUS_OK); i.size_bytes=5U; CHECK(umi_dr_resource_pack_add(&p,&i)==UMI_STATUS_OK); CHECK(p.total_bytes==5U);
    return 0;
}
