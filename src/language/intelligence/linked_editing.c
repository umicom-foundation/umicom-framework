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
#include "../../base/value_archive_internal.h"
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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->uri, '\0', sizeof(value->uri)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiLanguageIntelligenceLinkedEditingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb90a07d3e56daa29);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceLinkedEditing *)0)->uri)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiLanguageIntelligenceLinkedEditingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiLanguageIntelligenceLinkedEditing *)0)->uri) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiLanguageIntelligenceLinkedEditingArchiveWrite(UmiArchiveWriter *writer, const UmiLanguageIntelligenceLinkedEditing *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->uri, sizeof(value->uri));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->primary.start.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->primary.start.character);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->primary.end.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->primary.end.character);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->parent.start.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->parent.start.character);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->parent.end.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->parent.end.character);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->depth);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiLanguageIntelligenceLinkedEditingArchiveRead(UmiArchiveReader *reader, UmiLanguageIntelligenceLinkedEditing *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->uri, sizeof(value->uri));
    value->primary.start.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->primary.start.character = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->primary.end.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->primary.end.character = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->parent.start.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->parent.start.character = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->parent.end.line = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->parent.end.character = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->depth = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiLanguageIntelligenceLinkedEditingArchiveValidate(const UmiLanguageIntelligenceLinkedEditing *value)
{
    return umi_language_intelligence_linked_editing_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_language_intelligence_linked_editing_archive_encode, umi_language_intelligence_linked_editing_archive_decode,
    UmiLanguageIntelligenceLinkedEditing, UmiLanguageIntelligenceLinkedEditingArchiveSchema, UmiLanguageIntelligenceLinkedEditingArchiveBound, UmiLanguageIntelligenceLinkedEditingArchiveWrite, UmiLanguageIntelligenceLinkedEditingArchiveRead, UmiLanguageIntelligenceLinkedEditingArchiveValidate)
