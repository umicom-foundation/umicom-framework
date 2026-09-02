/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/types.c
 *
 * PURPOSE:
 *   shared frontend-conformance identifiers, enums, bounded text and deterministic scoring helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/types.h"

#include <string.h>

/* Provide the fc copy text operation used by this module and its client applications. */
UmiStatus umi_fc_copy_text(char *dst, size_t capacity, const char *src) {
    size_t len;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dst == NULL || capacity == 0U || src == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    len = strlen(src);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (len >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(dst, src, len + 1U);
    return UMI_STATUS_OK;
}
/* Provide the fc hash text operation used by this module and its client applications. */
uint64_t umi_fc_hash_text(const char *text) {
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *p = (const unsigned char *)text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*p != 0U) { hash ^= (uint64_t)*p++; hash *= UINT64_C(1099511628211); }
    return hash;
}
/* Provide the fc hash combine operation used by this module and its client applications. */
uint64_t umi_fc_hash_combine(uint64_t seed, uint64_t value) {
    return seed ^ (value + UINT64_C(0x9e3779b97f4a7c15) + (seed << 6U) + (seed >> 2U));
}
/* Provide the fc ratio operation used by this module and its client applications. */
double umi_fc_ratio(size_t numerator, size_t denominator) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (denominator == 0U) return numerator == 0U ? 1.0 : 0.0;
    return (double)numerator / (double)denominator;
}
/* Provide the fc clamp score operation used by this module and its client applications. */
double umi_fc_clamp_score(double score) { /* Apply this branch only when its contract condition is satisfied. */ if (score < 0.0) return 0.0; /* Apply this branch only when its contract condition is satisfied. */ if (score > 1.0) return 1.0; return score; }
/*
 * Provide the fc frontend kind text operation used by this module and its client
 * applications.
 */
const char *umi_fc_frontend_kind_text(UmiFcFrontendKind kind) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) { case UMI_FC_FRONTEND_GTK4: return "gtk4"; case UMI_FC_FRONTEND_QT6: return "qt6"; case UMI_FC_FRONTEND_NATIVE_WEB: return "native-web"; case UMI_FC_FRONTEND_HEADLESS: return "headless"; default: return "unknown"; }
}
/* Provide the fc outcome text operation used by this module and its client applications. */
const char *umi_fc_outcome_text(UmiFcOutcome outcome) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (outcome) { case UMI_FC_PASS: return "pass"; case UMI_FC_DEGRADED: return "degraded"; case UMI_FC_FAIL: return "fail"; default: return "unknown"; }
}
