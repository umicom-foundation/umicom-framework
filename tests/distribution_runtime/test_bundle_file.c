/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_bundle_file.c
 *
 * PURPOSE:
 *   Focused regression coverage for individual bundle-file path, size, checksum and executable metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_file.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrBundleFile value; umi_dr_bundle_file_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"app") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.path,sizeof(value.path),"bin/app") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.digest,sizeof(value.digest),"abc") == UMI_STATUS_OK); value.size_bytes=10U; CHECK(umi_dr_bundle_file_valid(&value)); CHECK(umi_dr_bundle_file_fingerprint(&value) != 0U);
    return 0;
}
