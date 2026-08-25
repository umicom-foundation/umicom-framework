/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_update_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for published update metadata, version, channel and package fingerprint.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrUpdateManifest value; umi_dr_update_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"upd")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"app")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.package_digest,sizeof(value.package_digest),"d")==UMI_STATUS_OK); value.channel=UMI_DR_CHANNEL_STABLE; CHECK(umi_dr_update_manifest_valid(&value)); CHECK(umi_dr_update_manifest_fingerprint(&value) != 0U);
    return 0;
}
