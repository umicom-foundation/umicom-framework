/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/source_navigation.h
 *
 * PURPOSE:
 *   Coordinate symbols, relationships, bookmarks, navigation history and
 *   provider-neutral definition/reference resolution behind one reusable API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SOURCE_NAVIGATION_H
#define UMICOM_EDITOR_SOURCE_NAVIGATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/navigation_history.h"
#include "umicom/editor/source_bookmark.h"
#include "umicom/editor/symbol_relationship.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SOURCE_NAVIGATION_API_VERSION 1U

/**
 * Represent the editor source navigation data shared with callers of this public contract.
 */
typedef struct UmiEditorSourceNavigation UmiEditorSourceNavigation;

/**
 * Represent the editor source navigation snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSourceNavigationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t symbol_count;
    size_t relationship_count;
    size_t bookmark_count;
    size_t history_count;
    uint64_t symbol_revision;
    uint64_t relationship_revision;
    uint64_t bookmark_revision;
    uint64_t history_revision;
    uint64_t revision;
    int can_go_back;
    int can_go_forward;
} UmiEditorSourceNavigationSnapshot;

/**
 * Initialise editor source navigation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_source_navigation_create(
    UmiEditorSourceNavigation **out_navigation);
/**
 * Release or reset state held by editor source navigation so the same storage can be
 * reused safely.
 */
void umi_editor_source_navigation_destroy(
    UmiEditorSourceNavigation *navigation);
/**
 * Provide the editor source navigation symbols operation used by this module and its
 * client applications.
 */
UmiEditorSymbolIndex *umi_editor_source_navigation_symbols(
    UmiEditorSourceNavigation *navigation);
/**
 * Provide the editor source navigation relationships operation used by this module and its
 * client applications.
 */
UmiEditorSymbolRelationshipGraph *umi_editor_source_navigation_relationships(
    UmiEditorSourceNavigation *navigation);
/**
 * Provide the editor source navigation history operation used by this module and its
 * client applications.
 */
UmiEditorNavigationHistory *umi_editor_source_navigation_history(
    UmiEditorSourceNavigation *navigation);
/**
 * Provide the editor source navigation bookmarks operation used by this module and its
 * client applications.
 */
UmiEditorSourceBookmarkRegistry *umi_editor_source_navigation_bookmarks(
    UmiEditorSourceNavigation *navigation);
/**
 * Provide the editor source navigation open operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_source_navigation_open(
    UmiEditorSourceNavigation *navigation,
    const UmiEditorSourceLocation *from,
    const UmiEditorSourceLocation *target,
    UmiEditorSourceLocation *out_target);
/**
 * Provide the editor source navigation resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_navigation_resolve(
    UmiEditorSourceNavigation *navigation,
    const char *symbol_id,
    UmiEditorNavigationQueryKind query_kind,
    UmiEditorNavigationResultSet *out_results);
/**
 * Provide the editor source navigation go back operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_navigation_go_back(
    UmiEditorSourceNavigation *navigation,
    UmiEditorSourceLocation *out_location);
/**
 * Provide the editor source navigation go forward operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_navigation_go_forward(
    UmiEditorSourceNavigation *navigation,
    UmiEditorSourceLocation *out_location);
/**
 * Provide the editor source navigation snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_navigation_snapshot(
    const UmiEditorSourceNavigation *navigation,
    UmiEditorSourceNavigationSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SOURCE_NAVIGATION_H */
