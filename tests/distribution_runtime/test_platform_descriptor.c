/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_platform_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for runtime operating-system descriptors and minimum platform revision requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/platform_descriptor.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrPlatformDescriptor value; umi_dr_platform_descriptor_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"linux") == UMI_STATUS_OK); value.platform=UMI_DR_PLATFORM_LINUX; CHECK(umi_dr_platform_descriptor_valid(&value)); CHECK(umi_dr_platform_descriptor_fingerprint(&value) != 0U);
    return 0;
}
