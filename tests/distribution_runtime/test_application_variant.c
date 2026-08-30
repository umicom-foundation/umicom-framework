/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_application_variant.c
 *
 * PURPOSE:
 *   Focused regression coverage for platform-specific application variants without moving reusable logic into products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_variant.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrApplicationVariant value; umi_dr_application_variant_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"win-x64") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.entrypoint,sizeof(value.entrypoint),"bin/app.exe") == UMI_STATUS_OK); CHECK(umi_dr_application_variant_valid(&value)); CHECK(umi_dr_application_variant_fingerprint(&value) != 0U);
    return 0;
}
