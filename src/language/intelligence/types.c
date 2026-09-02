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

/*
 * Provide the position before or equal operation used by this module and its client
 * applications.
 */
static int position_before_or_equal(
    UmiLanguageIntelligencePosition left,
    UmiLanguageIntelligencePosition right)
{
    return left.line < right.line ||
        (left.line == right.line && left.character <= right.character);
}

/*
 * Provide the language intelligence types init range operation used by this module and its
 * client applications.
 */
void umi_language_intelligence_types_init_range(
    UmiLanguageIntelligenceRange *range,
    uint32_t start_line,
    uint32_t start_character,
    uint32_t end_line,
    uint32_t end_character)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (range == NULL) {
        return;
    }
    range->start.line = start_line;
    range->start.character = start_character;
    range->end.line = end_line;
    range->end.character = end_character;
}

/*
 * Check that language intelligence range satisfies its contract before another service
 * relies on it.
 */
int umi_language_intelligence_range_is_valid(
    const UmiLanguageIntelligenceRange *range)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (range == NULL) {
        return 0;
    }
    return position_before_or_equal(range->start, range->end);
}

/*
 * Provide the language intelligence range contains operation used by this module and its
 * client applications.
 */
int umi_language_intelligence_range_contains(
    const UmiLanguageIntelligenceRange *outer,
    const UmiLanguageIntelligenceRange *inner)
{
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_language_intelligence_range_is_valid(outer) ||
        !umi_language_intelligence_range_is_valid(inner)) {
        return 0;
    }
    return position_before_or_equal(outer->start, inner->start) &&
        position_before_or_equal(inner->end, outer->end);
}

/*
 * Provide the language intelligence copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_intelligence_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Check that language intelligence text satisfies its contract before another service
 * relies on it.
 */
int umi_language_intelligence_text_is_valid(const char *text)
{
    return text != NULL && text[0] != '\0';
}

/*
 * Provide the language intelligence hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_language_intelligence_hash_text(const char *text)
{
    const unsigned char *cursor;
    uint64_t hash = UINT64_C(14695981039346656037);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) {
        return 0U;
    }
    cursor = (const unsigned char *)text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        hash ^= (uint64_t)(*cursor++);
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/*
 * Add language intelligence score saturating only after its inputs and available capacity
 * have been checked.
 */
uint64_t umi_language_intelligence_score_saturating_add(
    uint64_t left,
    uint64_t right)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (UINT64_MAX - left < right) {
        return UINT64_MAX;
    }
    return left + right;
}

/*
 * Provide the language intelligence state text operation used by this module and its
 * client applications.
 */
const char *umi_language_intelligence_state_text(
    UmiLanguageIntelligenceState state)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the language intelligence severity text operation used by this module and its
 * client applications.
 */
const char *umi_language_intelligence_severity_text(
    UmiLanguageIntelligenceSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_LANGUAGE_INTELLIGENCE_SEVERITY_INFORMATION: return "information";
        case UMI_LANGUAGE_INTELLIGENCE_SEVERITY_WARNING: return "warning";
        case UMI_LANGUAGE_INTELLIGENCE_SEVERITY_ERROR: return "error";
        case UMI_LANGUAGE_INTELLIGENCE_SEVERITY_FATAL: return "fatal";
        default: return "invalid";
    }
}
