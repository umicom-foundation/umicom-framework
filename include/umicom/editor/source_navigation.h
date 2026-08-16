/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/source_navigation.h
 *
 * PURPOSE:
 *   Coordinate symbols, relationships, bookmarks, navigation history and
 *   provider-neutral definition/reference resolution behind one reusable API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiEditorSourceNavigation UmiEditorSourceNavigation;

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

UmiStatus umi_editor_source_navigation_create(
    UmiEditorSourceNavigation **out_navigation);
void umi_editor_source_navigation_destroy(
    UmiEditorSourceNavigation *navigation);
UmiEditorSymbolIndex *umi_editor_source_navigation_symbols(
    UmiEditorSourceNavigation *navigation);
UmiEditorSymbolRelationshipGraph *umi_editor_source_navigation_relationships(
    UmiEditorSourceNavigation *navigation);
UmiEditorNavigationHistory *umi_editor_source_navigation_history(
    UmiEditorSourceNavigation *navigation);
UmiEditorSourceBookmarkRegistry *umi_editor_source_navigation_bookmarks(
    UmiEditorSourceNavigation *navigation);
UmiStatus umi_editor_source_navigation_open(
    UmiEditorSourceNavigation *navigation,
    const UmiEditorSourceLocation *from,
    const UmiEditorSourceLocation *target,
    UmiEditorSourceLocation *out_target);
UmiStatus umi_editor_source_navigation_resolve(
    UmiEditorSourceNavigation *navigation,
    const char *symbol_id,
    UmiEditorNavigationQueryKind query_kind,
    UmiEditorNavigationResultSet *out_results);
UmiStatus umi_editor_source_navigation_go_back(
    UmiEditorSourceNavigation *navigation,
    UmiEditorSourceLocation *out_location);
UmiStatus umi_editor_source_navigation_go_forward(
    UmiEditorSourceNavigation *navigation,
    UmiEditorSourceLocation *out_location);
UmiStatus umi_editor_source_navigation_snapshot(
    const UmiEditorSourceNavigation *navigation,
    UmiEditorSourceNavigationSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SOURCE_NAVIGATION_H */
