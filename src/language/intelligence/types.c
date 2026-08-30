/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/types.c
 *
 * PURPOSE:
 *   Implement shared language-intelligence primitives and deterministic helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/types.h"

#include <limits.h>
#include <string.h>

static int position_before_or_equal(
    UmiLanguageIntelligencePosition left,
    UmiLanguageIntelligencePosition right)
{
    return left.line < right.line ||
        (left.line == right.line && left.character <= right.character);
}

void umi_language_intelligence_types_init_range(
    UmiLanguageIntelligenceRange *range,
    uint32_t start_line,
    uint32_t start_character,
    uint32_t end_line,
    uint32_t end_character)
{
    if (range == NULL) {
        return;
    }
    range->start.line = start_line;
    range->start.character = start_character;
    range->end.line = end_line;
    range->end.character = end_character;
}

int umi_language_intelligence_range_is_valid(
    const UmiLanguageIntelligenceRange *range)
{
    if (range == NULL) {
        return 0;
    }
    return position_before_or_equal(range->start, range->end);
}

int umi_language_intelligence_range_contains(
    const UmiLanguageIntelligenceRange *outer,
    const UmiLanguageIntelligenceRange *inner)
{
    if (!umi_language_intelligence_range_is_valid(outer) ||
        !umi_language_intelligence_range_is_valid(inner)) {
        return 0;
    }
    return position_before_or_equal(outer->start, inner->start) &&
        position_before_or_equal(inner->end, outer->end);
}

UmiStatus umi_language_intelligence_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

int umi_language_intelligence_text_is_valid(const char *text)
{
    return text != NULL && text[0] != '\0';
}

uint64_t umi_language_intelligence_hash_text(const char *text)
{
    const unsigned char *cursor;
    uint64_t hash = UINT64_C(14695981039346656037);
    if (text == NULL) {
        return 0U;
    }
    cursor = (const unsigned char *)text;
    while (*cursor != 0U) {
        hash ^= (uint64_t)(*cursor++);
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

uint64_t umi_language_intelligence_score_saturating_add(
    uint64_t left,
    uint64_t right)
{
    if (UINT64_MAX - left < right) {
        return UINT64_MAX;
    }
    return left + right;
}

const char *umi_language_intelligence_state_text(
    UmiLanguageIntelligenceState state)
{
    switch (state) {
        case UMI_LANGUAGE_INTELLIGENCE_UNKNOWN: return "unknown";
        case UMI_LANGUAGE_INTELLIGENCE_STARTING: return "starting";
        case UMI_LANGUAGE_INTELLIGENCE_READY: return "ready";
        case UMI_LANGUAGE_INTELLIGENCE_DEGRADED: return "degraded";
        case UMI_LANGUAGE_INTELLIGENCE_QUARANTINED: return "quarantined";
        case UMI_LANGUAGE_INTELLIGENCE_STOPPED: return "stopped";
        case UMI_LANGUAGE_INTELLIGENCE_FAILED: return "failed";
        default: return "invalid";
    }
}

const char *umi_language_intelligence_severity_text(
    UmiLanguageIntelligenceSeverity severity)
{
    switch (severity) {
        case UMI_LANGUAGE_INTELLIGENCE_SEVERITY_INFORMATION: return "information";
        case UMI_LANGUAGE_INTELLIGENCE_SEVERITY_WARNING: return "warning";
        case UMI_LANGUAGE_INTELLIGENCE_SEVERITY_ERROR: return "error";
        case UMI_LANGUAGE_INTELLIGENCE_SEVERITY_FATAL: return "fatal";
        default: return "invalid";
    }
}
