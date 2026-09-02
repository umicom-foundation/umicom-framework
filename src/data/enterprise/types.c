/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/types.c
 *
 * PURPOSE:
 *   Define stable C23 types, capacities, states, value kinds and utility helpers shared by the enterprise Data Server platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/types.h"
#include <string.h>


/* Copying through this helper keeps all new enterprise data contracts bounded. */
UmiStatus umi_data_enterprise_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* FNV-1a is used here because it is deterministic, dependency-free and sufficient for non-cryptographic identity evidence. */
uint64_t umi_data_enterprise_hash64(const void *data, size_t size, uint64_t seed) {
    const unsigned char *bytes = (const unsigned char *)data;
    uint64_t hash = seed != 0U ? seed : UINT64_C(1469598103934665603);
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data == NULL && size != 0U) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < size; ++i) { hash ^= (uint64_t)bytes[i]; hash *= UINT64_C(1099511628211); }
    return hash;
}

/* Percent calculations are capped to keep health projections predictable. */
double umi_data_enterprise_percent(uint64_t numerator, uint64_t denominator) {
    double value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (denominator == 0U) return 0.0;
    value = ((double)numerator * 100.0) / (double)denominator;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value < 0.0) return 0.0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value > 100.0) return 100.0;
    return value;
}

/* Text labels are stable for diagnostics, CLI output and frontend projections. */
const char *umi_data_enterprise_state_text(UmiDataEnterpriseState state) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_DATA_ENTERPRISE_READY: return "ready";
        case UMI_DATA_ENTERPRISE_DEGRADED: return "degraded";
        case UMI_DATA_ENTERPRISE_BLOCKED: return "blocked";
        case UMI_DATA_ENTERPRISE_FAILED: return "failed";
        case UMI_DATA_ENTERPRISE_UNKNOWN:
        default: return "unknown";
    }
}
