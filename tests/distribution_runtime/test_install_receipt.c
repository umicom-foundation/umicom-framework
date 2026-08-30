/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_install_receipt.c
 *
 * PURPOSE:
 *   Focused regression coverage for immutable installation receipt and package fingerprint evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/install_receipt.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrInstallReceipt value; umi_dr_install_receipt_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"receipt")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"app")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.package_digest,sizeof(value.package_digest),"d")==UMI_STATUS_OK); value.installed_at=1U; CHECK(umi_dr_install_receipt_valid(&value)); CHECK(umi_dr_install_receipt_fingerprint(&value) != 0U);
    return 0;
}
