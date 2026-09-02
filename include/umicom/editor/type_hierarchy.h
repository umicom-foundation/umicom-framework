/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/type_hierarchy.h
 *
 * PURPOSE:
 *   Build supertype and subtype trees from the Framework symbol index and
 *   relationship graph using the shared hierarchy traversal engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_TYPE_HIERARCHY_H
#define UMICOM_EDITOR_TYPE_HIERARCHY_H

#include "umicom/editor/symbol_hierarchy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_TYPE_HIERARCHY_API_VERSION 1U

/**
 * List the named editor type hierarchy direction values accepted by this public contract.
 */
typedef enum UmiEditorTypeHierarchyDirection {
    UMI_EDITOR_TYPE_HIERARCHY_SUPERTYPES = 1,
    UMI_EDITOR_TYPE_HIERARCHY_SUBTYPES = 2
} UmiEditorTypeHierarchyDirection;

/**
 * Provide the editor type hierarchy build operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_type_hierarchy_build(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    UmiEditorTypeHierarchyDirection direction,
    const UmiEditorSymbolHierarchyBuildOptions *options);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_TYPE_HIERARCHY_H */
