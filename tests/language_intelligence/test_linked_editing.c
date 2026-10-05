/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_linked_editing.c
 * PURPOSE: Focused regression test for linked editing.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/language/intelligence/linked_editing.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/language/intelligence/linked_editing.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiLanguageIntelligenceLinkedEditingTransferEqual(const UmiLanguageIntelligenceLinkedEditing *a, const UmiLanguageIntelligenceLinkedEditing *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        strcmp(a->uri, b->uri) == 0 &&
        a->primary.start.line == b->primary.start.line &&
        a->primary.start.character == b->primary.start.character &&
        a->primary.end.line == b->primary.end.line &&
        a->primary.end.character == b->primary.end.character &&
        a->parent.start.line == b->parent.start.line &&
        a->parent.start.character == b->parent.start.character &&
        a->parent.end.line == b->parent.end.line &&
        a->parent.end.character == b->parent.end.character &&
        a->depth == b->depth &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiLanguageIntelligenceLinkedEditingTransferTails(UmiLanguageIntelligenceLinkedEditing *value)
{
    (void)value;
    {
        size_t used = strlen(value->uri) + 1U;
        memset(value->uri + used, 0xa5, sizeof(value->uri) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiLanguageIntelligenceLinkedEditingTransferMalformed(const UmiLanguageIntelligenceLinkedEditing *sample)
{
    (void)sample;
    {
        UmiLanguageIntelligenceLinkedEditing invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.uri, 'x', sizeof(invalid.uri));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_language_intelligence_linked_editing_validate(&invalid) != UMI_STATUS_OK) ||
            umi_language_intelligence_linked_editing_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated uri was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiLanguageIntelligenceLinkedEditingTransferCases, UmiLanguageIntelligenceLinkedEditing,
    umi_language_intelligence_linked_editing_archive_encode, umi_language_intelligence_linked_editing_archive_decode,
    UmiLanguageIntelligenceLinkedEditingTransferEqual, UmiLanguageIntelligenceLinkedEditingTransferTails, UmiLanguageIntelligenceLinkedEditingTransferMalformed)

int main(void)
{
    UmiLanguageIntelligenceLinkedEditing value;
    UmiLanguageIntelligenceRange primary;
    UmiLanguageIntelligenceRange parent;
    umi_language_intelligence_linked_editing_init(&value, "file:///test.c");
    umi_language_intelligence_types_init_range(&parent, 1U, 0U, 10U, 0U);
    umi_language_intelligence_types_init_range(&primary, 2U, 0U, 3U, 5U);
    CHECK(umi_language_intelligence_linked_editing_set_ranges(&value, &primary, &parent) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_linked_editing_validate(&value) == UMI_STATUS_OK);
    if (UmiLanguageIntelligenceLinkedEditingTransferCases(&value) != 0) return 1;

    CHECK(umi_language_intelligence_linked_editing_is_nested(&value) != 0);
    return 0;
}
