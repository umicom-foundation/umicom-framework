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

/**
 * List the named editor navigation experience surface values accepted by this public
 * contract.
 */
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

/**
 * List the named editor navigation resolution state values accepted by this public
 * contract.
 */
typedef enum UmiEditorNavigationResolutionState {
    UMI_EDITOR_NAVIGATION_RESOLUTION_NONE = 0,
    UMI_EDITOR_NAVIGATION_RESOLUTION_DIRECT = 1,
    UMI_EDITOR_NAVIGATION_RESOLUTION_MULTIPLE = 2,
    UMI_EDITOR_NAVIGATION_RESOLUTION_NOT_FOUND = 3,
    UMI_EDITOR_NAVIGATION_RESOLUTION_CANCELLED = 4,
    UMI_EDITOR_NAVIGATION_RESOLUTION_FAILED = 5
} UmiEditorNavigationResolutionState;

/**
 * Represent the editor navigation resolution data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor navigation experience snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor navigation experience data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorNavigationExperience
    UmiEditorNavigationExperience;

/**
 * Initialise editor navigation experience from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_navigation_experience_create(
    UmiEditorNavigationExperience **out_experience);
/**
 * Release or reset state held by editor navigation experience so the same storage can be
 * reused safely.
 */
void umi_editor_navigation_experience_destroy(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience providers operation used by this module and its
 * client applications.
 */
UmiEditorNavigationProviderRegistry *umi_editor_navigation_experience_providers(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience register provider operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_experience_register_provider(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationProviderRegistration *registration);
/**
 * Provide the editor navigation experience unregister provider operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_navigation_experience_unregister_provider(
    UmiEditorNavigationExperience *experience,
    const char *provider_id);
/**
 * Provide the editor navigation experience resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_resolve(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request,
    UmiEditorNavigationResolution *out_resolution);
/**
 * Provide the editor navigation experience open peek operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_open_peek(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request);
/**
 * Provide the editor navigation experience open references operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_experience_open_references(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request);
/**
 * Provide the editor navigation experience open hierarchy operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_experience_open_hierarchy(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationHierarchyRequest *request);
/**
 * Provide the editor navigation experience open symbols operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_experience_open_symbols(
    UmiEditorNavigationExperience *experience,
    UmiEditorSymbolNavigationScope scope,
    const UmiEditorNavigationRequest *request);
/**
 * Provide the editor navigation experience refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_refresh(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_cancel(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience close active operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_experience_close_active(
    UmiEditorNavigationExperience *experience,
    int force);
/**
 * Provide the editor navigation experience selected target operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_experience_selected_target(
    const UmiEditorNavigationExperience *experience,
    UmiEditorSourceLocation *out_location);
/**
 * Provide the editor navigation experience active surface operation used by this module
 * and its client applications.
 */
UmiEditorNavigationExperienceSurface umi_editor_navigation_experience_active_surface(
    const UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience resolution session operation used by this
 * module and its client applications.
 */
UmiEditorNavigationQuerySession *umi_editor_navigation_experience_resolution_session(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience peek session operation used by this module and
 * its client applications.
 */
UmiEditorPeekNavigationSession *umi_editor_navigation_experience_peek_session(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience reference session operation used by this module
 * and its client applications.
 */
UmiEditorReferenceNavigationSession *
umi_editor_navigation_experience_reference_session(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience call hierarchy session operation used by this
 * module and its client applications.
 */
UmiEditorHierarchyNavigationSession *
umi_editor_navigation_experience_call_hierarchy_session(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience type hierarchy session operation used by this
 * module and its client applications.
 */
UmiEditorHierarchyNavigationSession *
umi_editor_navigation_experience_type_hierarchy_session(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience symbol session operation used by this module
 * and its client applications.
 */
UmiEditorSymbolNavigationSession *
umi_editor_navigation_experience_symbol_session(
    UmiEditorNavigationExperience *experience);
/**
 * Provide the editor navigation experience snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_snapshot(
    const UmiEditorNavigationExperience *experience,
    UmiEditorNavigationExperienceSnapshot *out_snapshot);
/**
 * Provide the editor navigation experience revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_navigation_experience_revision(
    const UmiEditorNavigationExperience *experience);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_EXPERIENCE_H */
