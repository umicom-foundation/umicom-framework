/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_experience.h
 *
 * PURPOSE:
 *   Coordinate the complete source-navigation experience behind one reusable
 *   Framework service.  The service owns provider registration and the active
 *   resolution, peek, references, call hierarchy, type hierarchy, document
 *   symbol and workspace symbol sessions.  Applications contribute only menu,
 *   command, keybinding and view placement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_EXPERIENCE_H
#define UMICOM_EDITOR_NAVIGATION_EXPERIENCE_H

#include "umicom/editor/hierarchy_navigation_session.h"
#include "umicom/editor/peek_navigation_session.h"
#include "umicom/editor/reference_navigation_session.h"
#include "umicom/editor/symbol_navigation_session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_EXPERIENCE_API_VERSION 1U

typedef enum UmiEditorNavigationExperienceSurface {
    UMI_EDITOR_NAVIGATION_SURFACE_NONE = 0,
    UMI_EDITOR_NAVIGATION_SURFACE_RESOLUTION = 1,
    UMI_EDITOR_NAVIGATION_SURFACE_PEEK = 2,
    UMI_EDITOR_NAVIGATION_SURFACE_REFERENCES = 3,
    UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY = 4,
    UMI_EDITOR_NAVIGATION_SURFACE_TYPE_HIERARCHY = 5,
    UMI_EDITOR_NAVIGATION_SURFACE_DOCUMENT_SYMBOLS = 6,
    UMI_EDITOR_NAVIGATION_SURFACE_WORKSPACE_SYMBOLS = 7
} UmiEditorNavigationExperienceSurface;

typedef enum UmiEditorNavigationResolutionState {
    UMI_EDITOR_NAVIGATION_RESOLUTION_NONE = 0,
    UMI_EDITOR_NAVIGATION_RESOLUTION_DIRECT = 1,
    UMI_EDITOR_NAVIGATION_RESOLUTION_MULTIPLE = 2,
    UMI_EDITOR_NAVIGATION_RESOLUTION_NOT_FOUND = 3,
    UMI_EDITOR_NAVIGATION_RESOLUTION_CANCELLED = 4,
    UMI_EDITOR_NAVIGATION_RESOLUTION_FAILED = 5
} UmiEditorNavigationResolutionState;

typedef struct UmiEditorNavigationResolution {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationResolutionState state;
    UmiEditorNavigationQueryKind query_kind;
    size_t result_count;
    UmiEditorNavigationResult target;
    int has_target;
    int opened_surface;
} UmiEditorNavigationResolution;

typedef struct UmiEditorNavigationExperienceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationExperienceSurface active_surface;
    size_t provider_count;
    size_t active_item_count;
    size_t failed_provider_count;
    uint64_t provider_registry_revision;
    uint64_t active_session_revision;
    uint64_t revision;
    int busy;
    int cancellable;
    int has_selection;
} UmiEditorNavigationExperienceSnapshot;

typedef struct UmiEditorNavigationExperience
    UmiEditorNavigationExperience;

UmiStatus umi_editor_navigation_experience_create(
    UmiEditorNavigationExperience **out_experience);
void umi_editor_navigation_experience_destroy(
    UmiEditorNavigationExperience *experience);
UmiEditorNavigationProviderRegistry *umi_editor_navigation_experience_providers(
    UmiEditorNavigationExperience *experience);
UmiStatus umi_editor_navigation_experience_register_provider(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationProviderRegistration *registration);
UmiStatus umi_editor_navigation_experience_unregister_provider(
    UmiEditorNavigationExperience *experience,
    const char *provider_id);
UmiStatus umi_editor_navigation_experience_resolve(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request,
    UmiEditorNavigationResolution *out_resolution);
UmiStatus umi_editor_navigation_experience_open_peek(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request);
UmiStatus umi_editor_navigation_experience_open_references(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request);
UmiStatus umi_editor_navigation_experience_open_hierarchy(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationHierarchyRequest *request);
UmiStatus umi_editor_navigation_experience_open_symbols(
    UmiEditorNavigationExperience *experience,
    UmiEditorSymbolNavigationScope scope,
    const UmiEditorNavigationRequest *request);
UmiStatus umi_editor_navigation_experience_refresh(
    UmiEditorNavigationExperience *experience);
UmiStatus umi_editor_navigation_experience_cancel(
    UmiEditorNavigationExperience *experience);
UmiStatus umi_editor_navigation_experience_close_active(
    UmiEditorNavigationExperience *experience,
    int force);
UmiStatus umi_editor_navigation_experience_selected_target(
    const UmiEditorNavigationExperience *experience,
    UmiEditorSourceLocation *out_location);
UmiEditorNavigationExperienceSurface umi_editor_navigation_experience_active_surface(
    const UmiEditorNavigationExperience *experience);
UmiEditorNavigationQuerySession *umi_editor_navigation_experience_resolution_session(
    UmiEditorNavigationExperience *experience);
UmiEditorPeekNavigationSession *umi_editor_navigation_experience_peek_session(
    UmiEditorNavigationExperience *experience);
UmiEditorReferenceNavigationSession *
umi_editor_navigation_experience_reference_session(
    UmiEditorNavigationExperience *experience);
UmiEditorHierarchyNavigationSession *
umi_editor_navigation_experience_call_hierarchy_session(
    UmiEditorNavigationExperience *experience);
UmiEditorHierarchyNavigationSession *
umi_editor_navigation_experience_type_hierarchy_session(
    UmiEditorNavigationExperience *experience);
UmiEditorSymbolNavigationSession *
umi_editor_navigation_experience_symbol_session(
    UmiEditorNavigationExperience *experience);
UmiStatus umi_editor_navigation_experience_snapshot(
    const UmiEditorNavigationExperience *experience,
    UmiEditorNavigationExperienceSnapshot *out_snapshot);
uint64_t umi_editor_navigation_experience_revision(
    const UmiEditorNavigationExperience *experience);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_EXPERIENCE_H */
