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

bool umi_ct_id_valid(const char *text) {
    size_t i;
    if (text == NULL || text[0] == '\0') return false;
    for (i = 0U; text[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (!(isalnum(c) || c == '.' || c == '_' || c == '-' || c == ':')) return false;
        if (i + 1U >= UMI_CT_ID_CAPACITY) return false;
    }
    return true;
}

UmiStatus umi_ct_copy(char *destination, size_t capacity, const char *source) {
    size_t n;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    n = strlen(source);
    if (n >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, n + 1U);
    return UMI_STATUS_OK;
}

uint64_t umi_ct_hash_bytes(uint64_t seed, const void *data, size_t size) {
    const unsigned char *bytes = (const unsigned char *)data;
    uint64_t h = seed == 0U ? UINT64_C(1469598103934665603) : seed;
    size_t i;
    if (data == NULL && size != 0U) return 0U;
    for (i = 0U; i < size; ++i) { h ^= (uint64_t)bytes[i]; h *= UINT64_C(1099511628211); }
    return h;
}

uint64_t umi_ct_hash_text(uint64_t seed, const char *text) {
    if (text == NULL) return 0U;
    return umi_ct_hash_bytes(seed, text, strlen(text));
}

const char *umi_ct_support_level_text(UmiCtSupportLevel level) {
    switch (level) { case UMI_CT_SUPPORT_NONE: return "none"; case UMI_CT_SUPPORT_DEGRADED: return "degraded"; case UMI_CT_SUPPORT_NATIVE: return "native"; default: return "unknown"; }
}
const char *umi_ct_health_text(UmiCtHealth health) {
    switch (health) { case UMI_CT_HEALTH_UNKNOWN: return "unknown"; case UMI_CT_HEALTH_READY: return "ready"; case UMI_CT_HEALTH_DEGRADED: return "degraded"; case UMI_CT_HEALTH_BLOCKED: return "blocked"; default: return "invalid"; }
}
