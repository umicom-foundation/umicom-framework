/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/symbol_relationship.h
 *
 * PURPOSE:
 *   Model cross-document definitions, declarations, references, type links,
 *   implementations and call relationships independently of language provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SYMBOL_RELATIONSHIP_H
#define UMICOM_EDITOR_SYMBOL_RELATIONSHIP_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/navigation_result.h"
#include "umicom/editor/symbol_index.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SYMBOL_RELATIONSHIP_API_VERSION 1U
#define UMI_EDITOR_SYMBOL_RELATIONSHIP_ID_CAPACITY 128U

/**
 * List the named editor symbol relationship kind values accepted by this public contract.
 */
typedef enum UmiEditorSymbolRelationshipKind {
    UMI_EDITOR_SYMBOL_RELATIONSHIP_DEFINITION = 1,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_DECLARATION = 2,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_REFERENCE = 3,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_IMPLEMENTATION = 4,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_TYPE_DEFINITION = 5,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_BASE_TYPE = 6,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_DERIVED_TYPE = 7,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLER = 8,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_CALLEE = 9,
    UMI_EDITOR_SYMBOL_RELATIONSHIP_OVERRIDE = 10
} UmiEditorSymbolRelationshipKind;

/**
 * Represent the editor symbol relationship data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSymbolRelationship {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_SYMBOL_RELATIONSHIP_ID_CAPACITY];
    char source_symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char target_symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char provider_id[UMI_EDITOR_SYMBOL_PROVIDER_CAPACITY];
    UmiEditorSymbolRelationshipKind kind;
    UmiEditorSourceLocation source_location;
    UmiEditorSourceLocation target_location;
    int32_t rank;
    uint64_t revision;
} UmiEditorSymbolRelationship;

/**
 * Represent the editor symbol relationship graph data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSymbolRelationshipGraph
    UmiEditorSymbolRelationshipGraph;

/**
 * Initialise editor symbol relationship graph from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_symbol_relationship_graph_create(
    UmiEditorSymbolRelationshipGraph **out_graph);
/**
 * Release or reset state held by editor symbol relationship graph so the same storage can
 * be reused safely.
 */
void umi_editor_symbol_relationship_graph_destroy(
    UmiEditorSymbolRelationshipGraph *graph);
/**
 * Provide the editor symbol relationship graph upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_symbol_relationship_graph_upsert(
    UmiEditorSymbolRelationshipGraph *graph,
    const UmiEditorSymbolRelationship *relationship);
/**
 * Remove editor symbol relationship graph while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_editor_symbol_relationship_graph_remove(
    UmiEditorSymbolRelationshipGraph *graph,
    const char *relationship_id);
/**
 * Provide the editor symbol relationship graph remove symbol operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_symbol_relationship_graph_remove_symbol(
    UmiEditorSymbolRelationshipGraph *graph,
    const char *symbol_id,
    size_t *out_removed_count);
/**
 * Find editor symbol relationship graph while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_symbol_relationship_graph_find(
    const UmiEditorSymbolRelationshipGraph *graph,
    const char *relationship_id,
    UmiEditorSymbolRelationship *out_relationship);
/**
 * Find editor symbol relationship graph while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_symbol_relationship_graph_at(
    const UmiEditorSymbolRelationshipGraph *graph,
    size_t index,
    UmiEditorSymbolRelationship *out_relationship);
/**
 * Provide the editor symbol relationship graph collect operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_symbol_relationship_graph_collect(
    const UmiEditorSymbolRelationshipGraph *graph,
    const UmiEditorSymbolIndex *symbol_index,
    const char *symbol_id,
    UmiEditorSymbolRelationshipKind kind,
    int incoming,
    UmiEditorNavigationResultSet *out_results);
/**
 * Return the number of records represented by editor symbol relationship graph without
 * changing their state.
 */
size_t umi_editor_symbol_relationship_graph_count(
    const UmiEditorSymbolRelationshipGraph *graph);
/**
 * Provide the editor symbol relationship graph revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_symbol_relationship_graph_revision(
    const UmiEditorSymbolRelationshipGraph *graph);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SYMBOL_RELATIONSHIP_H */
