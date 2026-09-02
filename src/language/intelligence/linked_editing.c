/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/linked_editing.c
 *
 * PURPOSE:
 *   Implement represent linked-editing ranges and containment semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/linked_editing.h"
#include <string.h>
/*
 * Initialise language intelligence linked editing from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_linked_editing_init(UmiLanguageIntelligenceLinkedEditing *value, const char *uri)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_LINKED_EDITING_API_VERSION;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (uri != NULL) (void)umi_language_intelligence_copy_text(
        value->uri, sizeof(value->uri), uri);
}
/*
 * Provide the language intelligence linked editing set ranges operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_linked_editing_set_ranges(
    UmiLanguageIntelligenceLinkedEditing *value,
    const UmiLanguageIntelligenceRange *primary,
    const UmiLanguageIntelligenceRange *parent)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || primary == NULL || parent == NULL ||
        !umi_language_intelligence_range_is_valid(primary) ||
        !umi_language_intelligence_range_is_valid(parent) ||
        !umi_language_intelligence_range_contains(parent, primary))
        return UMI_STATUS_INVALID_ARGUMENT;
    value->primary = *primary;
    value->parent = *parent;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->revision != UINT64_MAX) value->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Check that language intelligence linked editing satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_linked_editing_validate(const UmiLanguageIntelligenceLinkedEditing *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->uri[0] == '\0' ||
        !umi_language_intelligence_range_is_valid(&value->primary) ||
        !umi_language_intelligence_range_is_valid(&value->parent) ||
        !umi_language_intelligence_range_contains(&value->parent, &value->primary))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the language intelligence linked editing is nested operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_linked_editing_is_nested(const UmiLanguageIntelligenceLinkedEditing *value)
{
    return umi_language_intelligence_linked_editing_validate(value) == UMI_STATUS_OK &&
        (value->primary.start.line != value->parent.start.line ||
         value->primary.start.character != value->parent.start.character ||
         value->primary.end.line != value->parent.end.line ||
         value->primary.end.character != value->parent.end.character);
}
