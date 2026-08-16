/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/call_hierarchy.h
 *
 * PURPOSE:
 *   Build incoming and outgoing call trees from the Framework symbol index and
 *   relationship graph using the shared hierarchy traversal engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_CALL_HIERARCHY_H
#define UMICOM_EDITOR_CALL_HIERARCHY_H

#include "umicom/editor/symbol_hierarchy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_CALL_HIERARCHY_API_VERSION 1U

typedef enum UmiEditorCallHierarchyDirection {
    UMI_EDITOR_CALL_HIERARCHY_INCOMING = 1,
    UMI_EDITOR_CALL_HIERARCHY_OUTGOING = 2
} UmiEditorCallHierarchyDirection;

UmiStatus umi_editor_call_hierarchy_build(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    UmiEditorCallHierarchyDirection direction,
    const UmiEditorSymbolHierarchyBuildOptions *options);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_CALL_HIERARCHY_H */
