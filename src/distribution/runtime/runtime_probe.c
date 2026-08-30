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

void umi_dr_runtime_probe_init(UmiDrRuntimeProbe *value) { if (value != NULL) { *value = (UmiDrRuntimeProbe){0}; value->platform=UMI_DR_PLATFORM_WINDOWS; value->architecture=UMI_DR_ARCH_X86_64; } }
bool umi_dr_runtime_probe_valid(const UmiDrRuntimeProbe *value) { return value != NULL && (value->id[0] != '\0' && value->platform != 0 && value->architecture != 0 && value->memory_mb > 0U); }
uint64_t umi_dr_runtime_probe_fingerprint(const UmiDrRuntimeProbe *value) {
    uint64_t h = 0U;
    if (!umi_dr_runtime_probe_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
