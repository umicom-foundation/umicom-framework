/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_type_hierarchy.c
 * PURPOSE: Focused regression test for type hierarchy.
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
#include "umicom/language/intelligence/type_hierarchy.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/language/intelligence/type_hierarchy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiLanguageIntelligenceTypeHierarchyEdgeTransferEqual(const UmiLanguageIntelligenceTypeHierarchyEdge *a, const UmiLanguageIntelligenceTypeHierarchyEdge *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->target_id, b->target_id) == 0 &&
        strcmp(a->relation, b->relation) == 0 &&
        a->weight == b->weight &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiLanguageIntelligenceTypeHierarchyEdgeTransferTails(UmiLanguageIntelligenceTypeHierarchyEdge *value)
{
    (void)value;
    {
        size_t used = strlen(value->source_id) + 1U;
        memset(value->source_id + used, 0xa5, sizeof(value->source_id) - used);
    }
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
    {
        size_t used = strlen(value->relation) + 1U;
        memset(value->relation + used, 0xa5, sizeof(value->relation) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiLanguageIntelligenceTypeHierarchyEdgeTransferMalformed(const UmiLanguageIntelligenceTypeHierarchyEdge *sample)
{
    (void)sample;
    {
        UmiLanguageIntelligenceTypeHierarchyEdge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_language_intelligence_type_hierarchy_edge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_language_intelligence_type_hierarchy_edge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiLanguageIntelligenceTypeHierarchyEdge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_language_intelligence_type_hierarchy_edge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_language_intelligence_type_hierarchy_edge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiLanguageIntelligenceTypeHierarchyEdge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.relation, 'x', sizeof(invalid.relation));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_language_intelligence_type_hierarchy_edge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_language_intelligence_type_hierarchy_edge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated relation was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiLanguageIntelligenceTypeHierarchyEdgeTransferCases, UmiLanguageIntelligenceTypeHierarchyEdge,
    umi_language_intelligence_type_hierarchy_edge_archive_encode, umi_language_intelligence_type_hierarchy_edge_archive_decode,
    UmiLanguageIntelligenceTypeHierarchyEdgeTransferEqual, UmiLanguageIntelligenceTypeHierarchyEdgeTransferTails, UmiLanguageIntelligenceTypeHierarchyEdgeTransferMalformed)

int main(void)
{
    UmiLanguageIntelligenceTypeHierarchyEdge edge;
    umi_language_intelligence_type_hierarchy_edge_init(&edge);
    CHECK(umi_language_intelligence_type_hierarchy_edge_set(&edge, "caller", "callee", "calls", 1U) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_type_hierarchy_edge_validate(&edge) == UMI_STATUS_OK);
    if (UmiLanguageIntelligenceTypeHierarchyEdgeTransferCases(&edge) != 0) return 1;

    CHECK(umi_language_intelligence_type_hierarchy_edge_matches_source(&edge, "caller") != 0);
    return 0;
}
