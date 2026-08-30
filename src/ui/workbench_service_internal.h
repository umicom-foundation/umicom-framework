/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workbench_service_internal.h
 *
 * PURPOSE:
 *   Share bounded case-insensitive ranking helpers between new workbench services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is private implementation support. Applications consume only public include/umicom/ui contracts.
 */

#ifndef UMICOM_UI_WORKBENCH_SERVICE_INTERNAL_H
#define UMICOM_UI_WORKBENCH_SERVICE_INTERNAL_H

#include <ctype.h>
#include <stddef.h>

static inline int umi_ui_ascii_equal_ci(char left, char right)
{
    return tolower((unsigned char)left) == tolower((unsigned char)right);
}

static inline int umi_ui_contains_ci(const char *haystack, const char *needle)
{
    size_t i;
    size_t j;
    if (haystack == NULL || needle == NULL) return 0;
    if (needle[0] == '\0') return 1;
    for (i = 0U; haystack[i] != '\0'; ++i) {
        for (j = 0U; needle[j] != '\0' && haystack[i + j] != '\0'; ++j) {
            if (!umi_ui_ascii_equal_ci(haystack[i + j], needle[j])) break;
        }
        if (needle[j] == '\0') return 1;
    }
    return 0;
}

static inline int umi_ui_fuzzy_score_ci(const char *query, const char *candidate)
{
    size_t qi = 0U;
    size_t ci;
    int score = 0;
    int consecutive = 0;
    if (query == NULL || candidate == NULL) return -1;
    if (query[0] == '\0') return 1;
    if (umi_ui_contains_ci(candidate, query)) score += 120;
    for (ci = 0U; candidate[ci] != '\0' && query[qi] != '\0'; ++ci) {
        if (umi_ui_ascii_equal_ci(query[qi], candidate[ci])) {
            ++qi;
            ++consecutive;
            score += 10 + consecutive * 3;
            if (ci == 0U || candidate[ci - 1U] == ' ' ||
                candidate[ci - 1U] == '-' || candidate[ci - 1U] == '_' ||
                candidate[ci - 1U] == '.' || candidate[ci - 1U] == '/' ||
                candidate[ci - 1U] == '\\') {
                score += 18;
            }
        } else {
            consecutive = 0;
        }
    }
    return query[qi] == '\0' ? score : -1;
}

#endif
