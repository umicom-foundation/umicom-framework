/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/type_hierarchy.c
 *
 * PURPOSE:
 *   Implement represent supertype/subtype hierarchy edges with stable identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/type_hierarchy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise language intelligence type hierarchy edge from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_type_hierarchy_edge_init(UmiLanguageIntelligenceTypeHierarchyEdge *edge)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edge == NULL) return;
    (void)memset(edge, 0, sizeof(*edge));
    edge->struct_size = (uint32_t)sizeof(*edge);
    edge->api_version = UMI_LANGUAGE_INTELLIGENCE_TYPE_HIERARCHY_API_VERSION;
    edge->enabled = 1;
}
/*
 * Copy language intelligence type hierarchy edge into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_language_intelligence_type_hierarchy_edge_set(
    UmiLanguageIntelligenceTypeHierarchyEdge *edge,
    const char *source_id,
    const char *target_id,
    const char *relation,
    uint32_t weight)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edge == NULL || source_id == NULL || target_id == NULL || relation == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(edge->source_id, sizeof(edge->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_intelligence_copy_text(edge->target_id, sizeof(edge->target_id), target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_intelligence_copy_text(edge->relation, sizeof(edge->relation), relation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    edge->weight = weight;
    return UMI_STATUS_OK;
}
/*
 * Check that language intelligence type hierarchy edge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_type_hierarchy_edge_validate(const UmiLanguageIntelligenceTypeHierarchyEdge *edge)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (edge == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(edge->source_id, '\0', sizeof(edge->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(edge->target_id, '\0', sizeof(edge->target_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(edge->relation, '\0', sizeof(edge->relation)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edge == NULL || edge->source_id[0] == '\0' ||
        edge->target_id[0] == '\0' || edge->relation[0] == '\0' ||
        edge->weight == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the language intelligence type hierarchy edge matches source operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_type_hierarchy_edge_matches_source(
    const UmiLanguageIntelligenceTypeHierarchyEdge *edge,
    const char *source_id)
{
    return umi_language_intelligence_type_hierarchy_edge_validate(edge) == UMI_STATUS_OK &&
        edge->enabled != 0 && source_id != NULL &&
        strcmp(edge->source_id, source_id) == 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiLanguageIntelligenceTypeHierarchyEdgeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa6b4e51b0a317967);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceTypeHierarchyEdge *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceTypeHierarchyEdge *)0)->target_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceTypeHierarchyEdge *)0)->relation)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiLanguageIntelligenceTypeHierarchyEdgeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiLanguageIntelligenceTypeHierarchyEdge *)0)->source_id) - 1U +
        8U + sizeof(((UmiLanguageIntelligenceTypeHierarchyEdge *)0)->target_id) - 1U +
        8U + sizeof(((UmiLanguageIntelligenceTypeHierarchyEdge *)0)->relation) - 1U +
        8U +
        8U;
}
static void UmiLanguageIntelligenceTypeHierarchyEdgeArchiveWrite(UmiArchiveWriter *writer, const UmiLanguageIntelligenceTypeHierarchyEdge *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteText(writer, value->relation, sizeof(value->relation));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->weight);
    UmiArchiveWriteSigned(writer, (int64_t)value->enabled);
}
static void UmiLanguageIntelligenceTypeHierarchyEdgeArchiveRead(UmiArchiveReader *reader, UmiLanguageIntelligenceTypeHierarchyEdge *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    UmiArchiveReadText(reader, value->relation, sizeof(value->relation));
    value->weight = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiLanguageIntelligenceTypeHierarchyEdgeArchiveValidate(const UmiLanguageIntelligenceTypeHierarchyEdge *value)
{
    return umi_language_intelligence_type_hierarchy_edge_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_language_intelligence_type_hierarchy_edge_archive_encode, umi_language_intelligence_type_hierarchy_edge_archive_decode,
    UmiLanguageIntelligenceTypeHierarchyEdge, UmiLanguageIntelligenceTypeHierarchyEdgeArchiveSchema, UmiLanguageIntelligenceTypeHierarchyEdgeArchiveBound, UmiLanguageIntelligenceTypeHierarchyEdgeArchiveWrite, UmiLanguageIntelligenceTypeHierarchyEdgeArchiveRead, UmiLanguageIntelligenceTypeHierarchyEdgeArchiveValidate)
