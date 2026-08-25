/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_profile.c
 *
 * PURPOSE:
 *   named runtime profiles combining platform, architecture and capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_profile.h"

void umi_dr_runtime_profile_init(UmiDrRuntimeProfile *value) { if (value != NULL) { *value = (UmiDrRuntimeProfile){0}; value->platform=UMI_DR_PLATFORM_LINUX; value->architecture=UMI_DR_ARCH_X86_64; } }
bool umi_dr_runtime_profile_valid(const UmiDrRuntimeProfile *value) { return value != NULL && (value->id[0] != '\0' && value->platform != 0 && value->architecture != 0); }
uint64_t umi_dr_runtime_profile_fingerprint(const UmiDrRuntimeProfile *value) {
    uint64_t h = 0U;
    if (!umi_dr_runtime_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
