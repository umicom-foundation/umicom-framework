/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_release_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for release identity, channel, platform matrix and artifact summary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/release_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrReleaseManifest value; umi_dr_release_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"rel")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"app")==UMI_STATUS_OK); value.channel=UMI_DR_CHANNEL_STABLE; value.artifact_count=1U; value.platform_count=1U; CHECK(umi_dr_release_manifest_valid(&value)); CHECK(umi_dr_release_manifest_fingerprint(&value) != 0U);
    return 0;
}
