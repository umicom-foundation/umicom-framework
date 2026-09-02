/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/symbol_search.c
 *
 * PURPOSE:
 *   Implement evaluate symbol-search matches with exact/prefix/substring ranking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/symbol_search.h"
#include <ctype.h>
#include <string.h>

/* Provide the equal char operation used by this module and its client applications. */
static int equal_char(char left, char right, int case_sensitive)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (case_sensitive != 0) return left == right;
    return tolower((unsigned char)left) == tolower((unsigned char)right);
}
/* Provide the starts with operation used by this module and its client applications. */
static int starts_with(const char *text, const char *prefix, int case_sensitive)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*prefix != '\0') {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (*text == '\0' || !equal_char(*text, *prefix, case_sensitive)) return 0;
        ++text;
        ++prefix;
    }
    return 1;
}
/* Provide the contains text operation used by this module and its client applications. */
static int contains_text(const char *text, const char *needle, int case_sensitive)
{
    const char *cursor;
    /* Apply this branch only when its contract condition is satisfied. */
    if (*needle == '\0') return 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (starts_with(cursor, needle, case_sensitive)) return 1;
    }
    return 0;
}
/*
 * Initialise language intelligence symbol search from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_symbol_search_init(UmiLanguageIntelligenceSymbolSearch *query, const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    query->struct_size = (uint32_t)sizeof(*query);
    query->api_version = UMI_LANGUAGE_INTELLIGENCE_SYMBOL_SEARCH_API_VERSION;
    query->limit = UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text != NULL) (void)umi_language_intelligence_copy_text(
        query->text, sizeof(query->text), text);
}
/*
 * Provide the language intelligence symbol search candidate score operation used by this
 * module and its client applications.
 */
uint64_t umi_language_intelligence_symbol_search_candidate_score(
    const UmiLanguageIntelligenceSymbolSearch *query,
    const char *candidate)
{
    size_t query_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || candidate == NULL || query->text[0] == '\0')
        return 0U;
    query_length = strlen(query->text);
    /* Apply this branch only when its contract condition is satisfied. */
    if (strlen(candidate) == query_length &&
        starts_with(candidate, query->text, query->case_sensitive))
        return UINT64_C(1000000);
    /* Apply this branch only when its contract condition is satisfied. */
    if (starts_with(candidate, query->text, query->case_sensitive))
        return UINT64_C(500000) + (uint64_t)query_length;
    /* Apply this branch only when its contract condition is satisfied. */
    if (contains_text(candidate, query->text, query->case_sensitive))
        return UINT64_C(100000) + (uint64_t)query_length;
    return 0U;
}
/*
 * Provide the language intelligence symbol search matches operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_symbol_search_matches(
    const UmiLanguageIntelligenceSymbolSearch *query,
    const char *candidate)
{
    return umi_language_intelligence_symbol_search_candidate_score(query, candidate) != 0U;
}
