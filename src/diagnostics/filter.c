/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/filter.c
 *
 * PURPOSE:
 *   Implement the filter behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/filter.h"

#include <ctype.h>
#include <string.h>

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *pattern)
{
    size_t text_length;
    size_t pattern_length;
    size_t index;
    size_t offset;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pattern == NULL || pattern[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    text_length = strlen(text);
    pattern_length = strlen(pattern);
    /* Apply this branch only when its contract condition is satisfied. */
    if (pattern_length > text_length) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index + pattern_length <= text_length; ++index) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U; offset < pattern_length; ++offset) {
            unsigned char left = (unsigned char)text[index + offset];
            unsigned char right = (unsigned char)pattern[offset];
            /* Apply this branch only when its contract condition is satisfied. */
            if (tolower(left) != tolower(right)) break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (offset == pattern_length) return 1;
    }
    return 0;
}

/*
 * Initialise diagnostic filter from caller-provided values so later operations receive a
 * known state.
 */
void umi_diagnostic_filter_init(UmiDiagnosticFilter *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->severity_mask = UINT32_MAX;
    filter->kind_mask = UINT32_MAX;
    filter->include_transient = 1;
}

/*
 * Provide the diagnostic filter set minimum severity operation used by this module and its
 * client applications.
 */
void umi_diagnostic_filter_set_minimum_severity(UmiDiagnosticFilter *filter,
                                                UmiDiagnosticSeverity severity)
{
    unsigned int index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL) return;
    filter->severity_mask = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = (unsigned int)severity; index < UMI_DIAGNOSTIC_SEVERITY_COUNT; ++index) {
        filter->severity_mask |= UINT32_C(1) << index;
    }
}

/*
 * Provide the diagnostic filter matches operation used by this module and its client
 * applications.
 */
int umi_diagnostic_filter_matches(const UmiDiagnosticFilter *filter,
                                  const UmiDiagnosticSnapshot *snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL || snapshot == NULL) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((filter->severity_mask & (UINT32_C(1) << (unsigned int)snapshot->severity)) == 0U ||
        (filter->kind_mask & (UINT32_C(1) << (unsigned int)snapshot->kind)) == 0U) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!filter->include_resolved && snapshot->resolved != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!filter->include_transient && snapshot->transient != 0) return 0;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (filter->source[0] != '\0' && strcmp(filter->source, snapshot->source) != 0) return 0;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (filter->owner[0] != '\0' && strcmp(filter->owner, snapshot->owner) != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->text[0] != '\0' &&
        !contains_case_insensitive(snapshot->message, filter->text) &&
        !contains_case_insensitive(snapshot->detail, filter->text) &&
        !contains_case_insensitive(snapshot->code, filter->text) &&
        !contains_case_insensitive(snapshot->uri, filter->text)) return 0;
    return 1;
}
