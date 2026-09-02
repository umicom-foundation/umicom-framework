/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_application_identity.c
 *
 * PURPOSE:
 *   Focused regression coverage for stable application identity, publisher and product-family metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_identity.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrApplicationIdentity value; umi_dr_application_identity_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"trader") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.publisher,sizeof(value.publisher),"Umicom Foundation") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.product,sizeof(value.product),"Umicom Trader") == UMI_STATUS_OK); CHECK(umi_dr_application_identity_valid(&value)); CHECK(umi_dr_application_identity_fingerprint(&value) != 0U);
    return 0;
}
