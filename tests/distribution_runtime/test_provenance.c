/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_provenance.c
 *
 * PURPOSE:
 *   Focused regression coverage for build/source/toolchain provenance evidence for packaged releases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/provenance.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrProvenance value; umi_dr_provenance_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"prov")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.source_revision,sizeof(value.source_revision),"rev")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.toolchain,sizeof(value.toolchain),"gcc-14")==UMI_STATUS_OK); CHECK(umi_dr_provenance_valid(&value)); CHECK(umi_dr_provenance_fingerprint(&value) != 0U);
    return 0;
}
