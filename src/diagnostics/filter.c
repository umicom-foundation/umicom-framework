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

static int contains_case_insensitive(const char *text, const char *pattern)
{
    size_t text_length;
    size_t pattern_length;
    size_t index;
    size_t offset;
    if (pattern == NULL || pattern[0] == '\0') return 1;
    if (text == NULL) return 0;
    text_length = strlen(text);
    pattern_length = strlen(pattern);
    if (pattern_length > text_length) return 0;
    for (index = 0U; index + pattern_length <= text_length; ++index) {
        for (offset = 0U; offset < pattern_length; ++offset) {
            unsigned char left = (unsigned char)text[index + offset];
            unsigned char right = (unsigned char)pattern[offset];
            if (tolower(left) != tolower(right)) break;
        }
        if (offset == pattern_length) return 1;
    }
    return 0;
}

void umi_diagnostic_filter_init(UmiDiagnosticFilter *filter)
{
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->severity_mask = UINT32_MAX;
    filter->kind_mask = UINT32_MAX;
    filter->include_transient = 1;
}

void umi_diagnostic_filter_set_minimum_severity(UmiDiagnosticFilter *filter,
                                                UmiDiagnosticSeverity severity)
{
    unsigned int index;
    if (filter == NULL) return;
    filter->severity_mask = 0U;
    for (index = (unsigned int)severity; index < UMI_DIAGNOSTIC_SEVERITY_COUNT; ++index) {
        filter->severity_mask |= UINT32_C(1) << index;
    }
}

int umi_diagnostic_filter_matches(const UmiDiagnosticFilter *filter,
                                  const UmiDiagnosticSnapshot *snapshot)
{
    if (filter == NULL || snapshot == NULL) return 0;
    if ((filter->severity_mask & (UINT32_C(1) << (unsigned int)snapshot->severity)) == 0U ||
        (filter->kind_mask & (UINT32_C(1) << (unsigned int)snapshot->kind)) == 0U) return 0;
    if (!filter->include_resolved && snapshot->resolved != 0) return 0;
    if (!filter->include_transient && snapshot->transient != 0) return 0;
    if (filter->source[0] != '\0' && strcmp(filter->source, snapshot->source) != 0) return 0;
    if (filter->owner[0] != '\0' && strcmp(filter->owner, snapshot->owner) != 0) return 0;
    if (filter->text[0] != '\0' &&
        !contains_case_insensitive(snapshot->message, filter->text) &&
        !contains_case_insensitive(snapshot->detail, filter->text) &&
        !contains_case_insensitive(snapshot->code, filter->text) &&
        !contains_case_insensitive(snapshot->uri, filter->text)) return 0;
    return 1;
}
