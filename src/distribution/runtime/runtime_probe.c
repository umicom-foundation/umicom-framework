/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_probe.c
 *
 * PURPOSE:
 *   deterministic probe snapshots describing detected host runtime properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_probe.h"

/*
 * Initialise dr runtime probe from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_runtime_probe_init(UmiDrRuntimeProbe *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrRuntimeProbe){0}; value->platform=UMI_DR_PLATFORM_WINDOWS; value->architecture=UMI_DR_ARCH_X86_64; } }
/* Check that dr runtime probe satisfies its contract before another service relies on it. */
bool umi_dr_runtime_probe_valid(const UmiDrRuntimeProbe *value) { return value != NULL && (value->id[0] != '\0' && value->platform != 0 && value->architecture != 0 && value->memory_mb > 0U); }
/*
 * Provide the dr runtime probe fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_runtime_probe_fingerprint(const UmiDrRuntimeProbe *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_runtime_probe_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
