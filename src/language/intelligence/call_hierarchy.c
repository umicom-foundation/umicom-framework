/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/call_hierarchy.c
 *
 * PURPOSE:
 *   Implement represent caller/callee hierarchy edges with stable identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/call_hierarchy.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise language intelligence call hierarchy edge from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_call_hierarchy_edge_init(UmiLanguageIntelligenceCallHierarchyEdge *edge)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edge == NULL) return;
    (void)memset(edge, 0, sizeof(*edge));
    edge->struct_size = (uint32_t)sizeof(*edge);
    edge->api_version = UMI_LANGUAGE_INTELLIGENCE_CALL_HIERARCHY_API_VERSION;
    edge->enabled = 1;
}
/*
 * Copy language intelligence call hierarchy edge into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_language_intelligence_call_hierarchy_edge_set(
    UmiLanguageIntelligenceCallHierarchyEdge *edge,
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
 * Check that language intelligence call hierarchy edge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_call_hierarchy_edge_validate(const UmiLanguageIntelligenceCallHierarchyEdge *edge)
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
 * Provide the language intelligence call hierarchy edge matches source operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_call_hierarchy_edge_matches_source(
    const UmiLanguageIntelligenceCallHierarchyEdge *edge,
    const char *source_id)
{
    return umi_language_intelligence_call_hierarchy_edge_validate(edge) == UMI_STATUS_OK &&
        edge->enabled != 0 && source_id != NULL &&
        strcmp(edge->source_id, source_id) == 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiLanguageIntelligenceCallHierarchyEdgeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x30df4e3586e0d04a);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceCallHierarchyEdge *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceCallHierarchyEdge *)0)->target_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiLanguageIntelligenceCallHierarchyEdge *)0)->relation)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiLanguageIntelligenceCallHierarchyEdgeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiLanguageIntelligenceCallHierarchyEdge *)0)->source_id) - 1U +
        8U + sizeof(((UmiLanguageIntelligenceCallHierarchyEdge *)0)->target_id) - 1U +
        8U + sizeof(((UmiLanguageIntelligenceCallHierarchyEdge *)0)->relation) - 1U +
        8U +
        8U;
}
static void UmiLanguageIntelligenceCallHierarchyEdgeArchiveWrite(UmiArchiveWriter *writer, const UmiLanguageIntelligenceCallHierarchyEdge *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteText(writer, value->relation, sizeof(value->relation));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->weight);
    UmiArchiveWriteSigned(writer, (int64_t)value->enabled);
}
static void UmiLanguageIntelligenceCallHierarchyEdgeArchiveRead(UmiArchiveReader *reader, UmiLanguageIntelligenceCallHierarchyEdge *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    UmiArchiveReadText(reader, value->relation, sizeof(value->relation));
    value->weight = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiLanguageIntelligenceCallHierarchyEdgeArchiveValidate(const UmiLanguageIntelligenceCallHierarchyEdge *value)
{
    return umi_language_intelligence_call_hierarchy_edge_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_language_intelligence_call_hierarchy_edge_archive_encode, umi_language_intelligence_call_hierarchy_edge_archive_decode,
    UmiLanguageIntelligenceCallHierarchyEdge, UmiLanguageIntelligenceCallHierarchyEdgeArchiveSchema, UmiLanguageIntelligenceCallHierarchyEdgeArchiveBound, UmiLanguageIntelligenceCallHierarchyEdgeArchiveWrite, UmiLanguageIntelligenceCallHierarchyEdgeArchiveRead, UmiLanguageIntelligenceCallHierarchyEdgeArchiveValidate)
