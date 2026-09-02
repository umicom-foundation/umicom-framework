/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/platform_descriptor.c
 *
 * PURPOSE:
 *   runtime operating-system descriptors and minimum platform revision requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/platform_descriptor.h"

/*
 * Initialise dr platform descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_platform_descriptor_init(UmiDrPlatformDescriptor *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrPlatformDescriptor){0}; value->platform=UMI_DR_PLATFORM_LINUX; value->desktop=true; } }
/*
 * Check that dr platform descriptor satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_platform_descriptor_valid(const UmiDrPlatformDescriptor *value) { return value != NULL && (value->id[0] != '\0' && value->platform >= UMI_DR_PLATFORM_WINDOWS && value->platform <= UMI_DR_PLATFORM_UMICOM_OS); }
/*
 * Provide the dr platform descriptor fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_platform_descriptor_fingerprint(const UmiDrPlatformDescriptor *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_platform_descriptor_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
