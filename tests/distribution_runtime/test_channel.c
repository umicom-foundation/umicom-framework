/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_channel.c
 *
 * PURPOSE:
 *   Focused regression coverage for release channel descriptors and stability ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/channel.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrChannel value; umi_dr_channel_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"stable")==UMI_STATUS_OK); CHECK(umi_dr_channel_valid(&value)); CHECK(umi_dr_channel_fingerprint(&value) != 0U);
    return 0;
}
