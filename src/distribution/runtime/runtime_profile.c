/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_profile.c
 *
 * PURPOSE:
 *   named runtime profiles combining platform, architecture and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_profile.h"

/*
 * Initialise dr runtime profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_runtime_profile_init(UmiDrRuntimeProfile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrRuntimeProfile){0}; value->platform=UMI_DR_PLATFORM_LINUX; value->architecture=UMI_DR_ARCH_X86_64; } }
/*
 * Check that dr runtime profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_runtime_profile_valid(const UmiDrRuntimeProfile *value) { return value != NULL && (value->id[0] != '\0' && value->platform != 0 && value->architecture != 0); }
/*
 * Provide the dr runtime profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_runtime_profile_fingerprint(const UmiDrRuntimeProfile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_runtime_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
