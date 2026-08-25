/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_application_bundle.c
 *
 * PURPOSE:
 *   Focused regression coverage for application bundle metadata, selected variant and immutable content fingerprint.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_bundle.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrApplicationBundle value; umi_dr_application_bundle_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"bundle") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"studio") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.variant_id,sizeof(value.variant_id),"win-x64") == UMI_STATUS_OK); value.file_count=1U; CHECK(umi_dr_application_bundle_valid(&value)); CHECK(umi_dr_application_bundle_fingerprint(&value) != 0U);
    return 0;
}
