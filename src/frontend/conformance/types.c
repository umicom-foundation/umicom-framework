/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/types.c
 *
 * PURPOSE:
 *   shared frontend-conformance identifiers, enums, bounded text and deterministic scoring helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/types.h"

#include <string.h>

UmiStatus umi_fc_copy_text(char *dst, size_t capacity, const char *src) {
    size_t len;
    if (dst == NULL || capacity == 0U || src == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    len = strlen(src);
    if (len >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(dst, src, len + 1U);
    return UMI_STATUS_OK;
}
uint64_t umi_fc_hash_text(const char *text) {
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *p = (const unsigned char *)text;
    if (text == NULL) return 0U;
    while (*p != 0U) { hash ^= (uint64_t)*p++; hash *= UINT64_C(1099511628211); }
    return hash;
}
uint64_t umi_fc_hash_combine(uint64_t seed, uint64_t value) {
    return seed ^ (value + UINT64_C(0x9e3779b97f4a7c15) + (seed << 6U) + (seed >> 2U));
}
double umi_fc_ratio(size_t numerator, size_t denominator) {
    if (denominator == 0U) return numerator == 0U ? 1.0 : 0.0;
    return (double)numerator / (double)denominator;
}
double umi_fc_clamp_score(double score) { if (score < 0.0) return 0.0; if (score > 1.0) return 1.0; return score; }
const char *umi_fc_frontend_kind_text(UmiFcFrontendKind kind) {
    switch (kind) { case UMI_FC_FRONTEND_GTK4: return "gtk4"; case UMI_FC_FRONTEND_QT6: return "qt6"; case UMI_FC_FRONTEND_NATIVE_WEB: return "native-web"; case UMI_FC_FRONTEND_HEADLESS: return "headless"; default: return "unknown"; }
}
const char *umi_fc_outcome_text(UmiFcOutcome outcome) {
    switch (outcome) { case UMI_FC_PASS: return "pass"; case UMI_FC_DEGRADED: return "degraded"; case UMI_FC_FAIL: return "fail"; default: return "unknown"; }
}
