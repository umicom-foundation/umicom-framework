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

typedef struct UmiEditorSymbolRelationshipGraph
    UmiEditorSymbolRelationshipGraph;

UmiStatus umi_editor_symbol_relationship_graph_create(
    UmiEditorSymbolRelationshipGraph **out_graph);
void umi_editor_symbol_relationship_graph_destroy(
    UmiEditorSymbolRelationshipGraph *graph);
UmiStatus umi_editor_symbol_relationship_graph_upsert(
    UmiEditorSymbolRelationshipGraph *graph,
    const UmiEditorSymbolRelationship *relationship);
UmiStatus umi_editor_symbol_relationship_graph_remove(
    UmiEditorSymbolRelationshipGraph *graph,
    const char *relationship_id);
UmiStatus umi_editor_symbol_relationship_graph_remove_symbol(
    UmiEditorSymbolRelationshipGraph *graph,
    const char *symbol_id,
    size_t *out_removed_count);
UmiStatus umi_editor_symbol_relationship_graph_find(
    const UmiEditorSymbolRelationshipGraph *graph,
    const char *relationship_id,
    UmiEditorSymbolRelationship *out_relationship);
UmiStatus umi_editor_symbol_relationship_graph_at(
    const UmiEditorSymbolRelationshipGraph *graph,
    size_t index,
    UmiEditorSymbolRelationship *out_relationship);
UmiStatus umi_editor_symbol_relationship_graph_collect(
    const UmiEditorSymbolRelationshipGraph *graph,
    const UmiEditorSymbolIndex *symbol_index,
    const char *symbol_id,
    UmiEditorSymbolRelationshipKind kind,
    int incoming,
    UmiEditorNavigationResultSet *out_results);
size_t umi_editor_symbol_relationship_graph_count(
    const UmiEditorSymbolRelationshipGraph *graph);
uint64_t umi_editor_symbol_relationship_graph_revision(
    const UmiEditorSymbolRelationshipGraph *graph);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SYMBOL_RELATIONSHIP_H */
