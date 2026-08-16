/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/type_hierarchy.h
 *
 * PURPOSE:
 *   Build supertype and subtype trees from the Framework symbol index and
 *   relationship graph using the shared hierarchy traversal engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_TYPE_HIERARCHY_H
#define UMICOM_EDITOR_TYPE_HIERARCHY_H

#include "umicom/editor/symbol_hierarchy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_TYPE_HIERARCHY_API_VERSION 1U

typedef enum UmiEditorTypeHierarchyDirection {
    UMI_EDITOR_TYPE_HIERARCHY_SUPERTYPES = 1,
    UMI_EDITOR_TYPE_HIERARCHY_SUBTYPES = 2
} UmiEditorTypeHierarchyDirection;

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
