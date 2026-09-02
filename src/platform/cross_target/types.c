/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/types.c
 *
 * PURPOSE:
 *   Define common cross-target identifiers, limits, support levels, health states and shared utilities.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/types.h"

#include <ctype.h>
#include <string.h>

/* Check that ct id satisfies its contract before another service relies on it. */
bool umi_ct_id_valid(const char *text) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0') return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; text[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)text[i];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!(isalnum(c) || c == '.' || c == '_' || c == '-' || c == ':')) return false;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (i + 1U >= UMI_CT_ID_CAPACITY) return false;
    }
    return true;
}

/* Copy ct into module-owned storage so callers keep ownership of their input values. */
UmiStatus umi_ct_copy(char *destination, size_t capacity, const char *source) {
    size_t n;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    n = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (n >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, n + 1U);
    return UMI_STATUS_OK;
}

/* Provide the ct hash bytes operation used by this module and its client applications. */
uint64_t umi_ct_hash_bytes(uint64_t seed, const void *data, size_t size) {
    const unsigned char *bytes = (const unsigned char *)data;
    uint64_t h = seed == 0U ? UINT64_C(1469598103934665603) : seed;
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data == NULL && size != 0U) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < size; ++i) { h ^= (uint64_t)bytes[i]; h *= UINT64_C(1099511628211); }
    return h;
}

/* Provide the ct hash text operation used by this module and its client applications. */
uint64_t umi_ct_hash_text(uint64_t seed, const char *text) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    return umi_ct_hash_bytes(seed, text, strlen(text));
}

/*
 * Provide the ct support level text operation used by this module and its client
 * applications.
 */
const char *umi_ct_support_level_text(UmiCtSupportLevel level) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (level) { case UMI_CT_SUPPORT_NONE: return "none"; case UMI_CT_SUPPORT_DEGRADED: return "degraded"; case UMI_CT_SUPPORT_NATIVE: return "native"; default: return "unknown"; }
}
/* Provide the ct health text operation used by this module and its client applications. */
const char *umi_ct_health_text(UmiCtHealth health) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (health) { case UMI_CT_HEALTH_UNKNOWN: return "unknown"; case UMI_CT_HEALTH_READY: return "ready"; case UMI_CT_HEALTH_DEGRADED: return "degraded"; case UMI_CT_HEALTH_BLOCKED: return "blocked"; default: return "invalid"; }
}
