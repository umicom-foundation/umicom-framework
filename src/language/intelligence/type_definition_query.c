/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/type_definition_query.c
 *
 * PURPOSE:
 *   Implement represent and score type-definition candidates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/type_definition_query.h"
#include <ctype.h>
#include <string.h>

static int equal_char(char left, char right, int case_sensitive)
{
    if (case_sensitive != 0) return left == right;
    return tolower((unsigned char)left) == tolower((unsigned char)right);
}
static int starts_with(const char *text, const char *prefix, int case_sensitive)
{
    while (*prefix != '\0') {
        if (*text == '\0' || !equal_char(*text, *prefix, case_sensitive)) return 0;
        ++text;
        ++prefix;
    }
    return 1;
}
static int contains_text(const char *text, const char *needle, int case_sensitive)
{
    const char *cursor;
    if (*needle == '\0') return 1;
    for (cursor = text; *cursor != '\0'; ++cursor) {
        if (starts_with(cursor, needle, case_sensitive)) return 1;
    }
    return 0;
}
void umi_language_intelligence_type_definition_query_init(UmiLanguageIntelligenceTypeDefinitionQuery *query, const char *text)
{
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    query->struct_size = (uint32_t)sizeof(*query);
    query->api_version = UMI_LANGUAGE_INTELLIGENCE_TYPE_DEFINITION_QUERY_API_VERSION;
    query->limit = UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY;
    if (text != NULL) (void)umi_language_intelligence_copy_text(
        query->text, sizeof(query->text), text);
}
uint64_t umi_language_intelligence_type_definition_query_candidate_score(
    const UmiLanguageIntelligenceTypeDefinitionQuery *query,
    const char *candidate)
{
    size_t query_length;
    if (query == NULL || candidate == NULL || query->text[0] == '\0')
        return 0U;
    query_length = strlen(query->text);
    if (strlen(candidate) == query_length &&
        starts_with(candidate, query->text, query->case_sensitive))
        return UINT64_C(1000000);
    if (starts_with(candidate, query->text, query->case_sensitive))
        return UINT64_C(500000) + (uint64_t)query_length;
    if (contains_text(candidate, query->text, query->case_sensitive))
        return UINT64_C(100000) + (uint64_t)query_length;
    return 0U;
}
int umi_language_intelligence_type_definition_query_matches(
    const UmiLanguageIntelligenceTypeDefinitionQuery *query,
    const char *candidate)
{
    return umi_language_intelligence_type_definition_query_candidate_score(query, candidate) != 0U;
}
