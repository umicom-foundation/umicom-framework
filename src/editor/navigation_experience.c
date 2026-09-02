/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_experience.c
 *
 * PURPOSE:
 *   Compose provider registration, direct navigation, peek, references,
 *   hierarchy and symbol sessions into one Framework-owned experience.  This is
 *   the application-facing coordination boundary; GUI products do not recreate
 *   provider routing or session state machines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/navigation_experience.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorNavigationExperience {
    UmiEditorNavigationProviderRegistry *providers;
    UmiEditorNavigationQuerySession *resolution;
    UmiEditorPeekNavigationSession *peek;
    UmiEditorReferenceNavigationSession *references;
    UmiEditorHierarchyNavigationSession *call_hierarchy;
    UmiEditorHierarchyNavigationSession *type_hierarchy;
    UmiEditorSymbolNavigationSession *symbols;
    UmiEditorNavigationExperienceSurface active_surface;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the destroy partial operation used by this module and its client applications. */
static void destroy_partial(UmiEditorNavigationExperience *experience)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return;
    umi_editor_symbol_navigation_session_destroy(experience->symbols);
    umi_editor_hierarchy_navigation_session_destroy(experience->type_hierarchy);
    umi_editor_hierarchy_navigation_session_destroy(experience->call_hierarchy);
    umi_editor_reference_navigation_session_destroy(experience->references);
    umi_editor_peek_navigation_session_destroy(experience->peek);
    umi_editor_navigation_query_session_destroy(experience->resolution);
    umi_editor_navigation_provider_registry_destroy(experience->providers);
    experience->symbols = NULL;
    experience->type_hierarchy = NULL;
    experience->call_hierarchy = NULL;
    experience->references = NULL;
    experience->peek = NULL;
    experience->resolution = NULL;
    experience->providers = NULL;
    free(experience);
}

/*
 * Initialise editor navigation experience from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_navigation_experience_create(
    UmiEditorNavigationExperience **out_experience)
{
    UmiEditorNavigationExperience *experience;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_experience = NULL;
    experience = (UmiEditorNavigationExperience *)calloc(1U,
                                                          sizeof(*experience));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_navigation_provider_registry_create(
        &experience->providers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_navigation_query_session_create(
            experience->providers, &experience->resolution);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_peek_navigation_session_create(
            experience->providers, &experience->peek);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_reference_navigation_session_create(
            experience->providers, &experience->references);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_hierarchy_navigation_session_create(
            experience->providers, &experience->call_hierarchy);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_hierarchy_navigation_session_create(
            experience->providers, &experience->type_hierarchy);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_symbol_navigation_session_create(
            experience->providers, &experience->symbols);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        destroy_partial(experience);
        return status;
    }
    experience->revision = 1U;
    *out_experience = experience;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor navigation experience so the same storage can be
 * reused safely.
 */
void umi_editor_navigation_experience_destroy(
    UmiEditorNavigationExperience *experience)
{
    destroy_partial(experience);
}

/*
 * Provide the editor navigation experience providers operation used by this module and its
 * client applications.
 */
UmiEditorNavigationProviderRegistry *umi_editor_navigation_experience_providers(
    UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->providers : NULL;
}

/*
 * Provide the editor navigation experience register provider operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_experience_register_provider(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationProviderRegistration *registration)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_provider_registry_upsert(
        experience->providers, registration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience unregister provider operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_navigation_experience_unregister_provider(
    UmiEditorNavigationExperience *experience,
    const char *provider_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_provider_registry_remove(
        experience->providers, provider_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_resolve(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request,
    UmiEditorNavigationResolution *out_resolution)
{
    UmiEditorNavigationQueryOptions options;
    size_t count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || request == NULL || out_resolution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_resolution, 0, sizeof(*out_resolution));
    out_resolution->struct_size = (uint32_t)sizeof(*out_resolution);
    out_resolution->api_version = UMI_EDITOR_NAVIGATION_EXPERIENCE_API_VERSION;
    out_resolution->query_kind = request->query_kind;
    (void)umi_editor_navigation_query_options_initialize(&options);
    options.maximum_results = request->maximum_results > 0U
        ? request->maximum_results
        : UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_RESULTS;
    status = umi_editor_navigation_query_session_execute(
        experience->resolution, request, &options);
    experience->active_surface = UMI_EDITOR_NAVIGATION_SURFACE_RESOLUTION;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CANCELLED) {
        out_resolution->state = UMI_EDITOR_NAVIGATION_RESOLUTION_CANCELLED;
        experience->revision = next_revision(experience->revision);
        return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        out_resolution->state = status == UMI_STATUS_NOT_FOUND
            ? UMI_EDITOR_NAVIGATION_RESOLUTION_NOT_FOUND
            : UMI_EDITOR_NAVIGATION_RESOLUTION_FAILED;
        experience->revision = next_revision(experience->revision);
        return status;
    }
    count = umi_editor_navigation_query_session_count(experience->resolution);
    out_resolution->result_count = count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 0U) {
        out_resolution->state = UMI_EDITOR_NAVIGATION_RESOLUTION_NOT_FOUND;
        experience->revision = next_revision(experience->revision);
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_editor_navigation_query_session_selected(
        experience->resolution, &out_resolution->target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_resolution->has_target = 1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 1U) {
        out_resolution->state = UMI_EDITOR_NAVIGATION_RESOLUTION_DIRECT;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_resolution->state = UMI_EDITOR_NAVIGATION_RESOLUTION_MULTIPLE;
        /* Apply this branch only when its contract condition is satisfied. */
        if (request->query_kind == UMI_EDITOR_NAVIGATION_QUERY_REFERENCE) {
            status = umi_editor_navigation_experience_open_references(
                experience, request);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = umi_editor_navigation_experience_open_peek(experience,
                                                                request);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) out_resolution->opened_surface = 1;
    }
    experience->revision = next_revision(experience->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor navigation experience open peek operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_open_peek(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_peek_navigation_session_open(experience->peek, request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->active_surface = UMI_EDITOR_NAVIGATION_SURFACE_PEEK;
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience open references operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_experience_open_references(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationRequest *request)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_reference_navigation_session_open(
        experience->references, request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->active_surface = UMI_EDITOR_NAVIGATION_SURFACE_REFERENCES;
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience open hierarchy operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_experience_open_hierarchy(
    UmiEditorNavigationExperience *experience,
    const UmiEditorNavigationHierarchyRequest *request)
{
    UmiEditorHierarchyNavigationSession *session;
    UmiEditorNavigationExperienceSurface surface;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || request == NULL ||
        request->kind < UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING ||
        request->kind > UMI_EDITOR_NAVIGATION_HIERARCHY_TYPE_SUBTYPES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->kind == UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING ||
        request->kind == UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_OUTGOING) {
        session = experience->call_hierarchy;
        surface = UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        session = experience->type_hierarchy;
        surface = UMI_EDITOR_NAVIGATION_SURFACE_TYPE_HIERARCHY;
    }
    status = umi_editor_hierarchy_navigation_session_open(session, request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->active_surface = surface;
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience open symbols operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_experience_open_symbols(
    UmiEditorNavigationExperience *experience,
    UmiEditorSymbolNavigationScope scope,
    const UmiEditorNavigationRequest *request)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_symbol_navigation_session_open(experience->symbols,
                                                       scope, request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->active_surface =
            scope == UMI_EDITOR_SYMBOL_NAVIGATION_DOCUMENT
                ? UMI_EDITOR_NAVIGATION_SURFACE_DOCUMENT_SYMBOLS
                : UMI_EDITOR_NAVIGATION_SURFACE_WORKSPACE_SYMBOLS;
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_refresh(
    UmiEditorNavigationExperience *experience)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Select the behaviour associated with the requested command or state value. */
    switch (experience->active_surface) {
        case UMI_EDITOR_NAVIGATION_SURFACE_PEEK:
            status = umi_editor_peek_navigation_session_refresh(
                experience->peek);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_REFERENCES:
            status = umi_editor_reference_navigation_session_refresh(
                experience->references);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY:
            status = umi_editor_hierarchy_navigation_session_refresh(
                experience->call_hierarchy);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_TYPE_HIERARCHY:
            status = umi_editor_hierarchy_navigation_session_refresh(
                experience->type_hierarchy);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_DOCUMENT_SYMBOLS:
        case UMI_EDITOR_NAVIGATION_SURFACE_WORKSPACE_SYMBOLS:
            status = umi_editor_symbol_navigation_session_refresh(
                experience->symbols);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_RESOLUTION:
            status = umi_editor_navigation_query_session_refresh(
                experience->resolution);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_NONE:
        default:
            return UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_cancel(
    UmiEditorNavigationExperience *experience)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Select the behaviour associated with the requested command or state value. */
    switch (experience->active_surface) {
        case UMI_EDITOR_NAVIGATION_SURFACE_RESOLUTION:
            status = umi_editor_navigation_query_session_cancel(
                experience->resolution);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_PEEK:
            status = umi_editor_peek_navigation_session_cancel(
                experience->peek);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_REFERENCES:
            status = umi_editor_reference_navigation_session_cancel(
                experience->references);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY:
            status = umi_editor_hierarchy_navigation_session_cancel(
                experience->call_hierarchy);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_TYPE_HIERARCHY:
            status = umi_editor_hierarchy_navigation_session_cancel(
                experience->type_hierarchy);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_DOCUMENT_SYMBOLS:
        case UMI_EDITOR_NAVIGATION_SURFACE_WORKSPACE_SYMBOLS:
            status = umi_editor_symbol_navigation_session_cancel(
                experience->symbols);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_NONE:
        default:
            return UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience close active operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_experience_close_active(
    UmiEditorNavigationExperience *experience,
    int force)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Select the behaviour associated with the requested command or state value. */
    switch (experience->active_surface) {
        case UMI_EDITOR_NAVIGATION_SURFACE_RESOLUTION:
            status = umi_editor_navigation_query_session_clear(
                experience->resolution);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_PEEK:
            status = umi_editor_peek_navigation_session_close(experience->peek,
                                                              force);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_REFERENCES:
            status = umi_editor_reference_navigation_session_close(
                experience->references);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY:
            status = umi_editor_hierarchy_navigation_session_close(
                experience->call_hierarchy);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_TYPE_HIERARCHY:
            status = umi_editor_hierarchy_navigation_session_close(
                experience->type_hierarchy);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_DOCUMENT_SYMBOLS:
        case UMI_EDITOR_NAVIGATION_SURFACE_WORKSPACE_SYMBOLS:
            status = umi_editor_symbol_navigation_session_close(
                experience->symbols);
            break;
        case UMI_EDITOR_NAVIGATION_SURFACE_NONE:
        default:
            return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        experience->active_surface = UMI_EDITOR_NAVIGATION_SURFACE_NONE;
        experience->revision = next_revision(experience->revision);
    }
    return status;
}

/*
 * Provide the editor navigation experience selected target operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_experience_selected_target(
    const UmiEditorNavigationExperience *experience,
    UmiEditorSourceLocation *out_location)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Select the behaviour associated with the requested command or state value. */
    switch (experience->active_surface) {
        case UMI_EDITOR_NAVIGATION_SURFACE_RESOLUTION: {
            UmiEditorNavigationResult result;
            UmiStatus status = umi_editor_navigation_query_session_selected(
                experience->resolution, &result);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) *out_location = result.location;
            return status;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_PEEK: {
            UmiEditorNavigationResult result;
            UmiStatus status = umi_editor_peek_navigation_session_active_result(
                experience->peek, &result);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) *out_location = result.location;
            return status;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_REFERENCES: {
            UmiEditorReferenceNavigationEntry entry;
            UmiStatus status = umi_editor_reference_navigation_session_selected(
                experience->references, &entry);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) *out_location = entry.result.location;
            return status;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY:
        case UMI_EDITOR_NAVIGATION_SURFACE_TYPE_HIERARCHY: {
            UmiEditorHierarchyNavigationEntry entry;
            UmiEditorHierarchyNavigationSession *session =
                experience->active_surface ==
                        UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY
                    ? experience->call_hierarchy
                    : experience->type_hierarchy;
            UmiStatus status =
                umi_editor_hierarchy_navigation_session_selected(session,
                                                                 &entry);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) *out_location = entry.node.location;
            return status;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_DOCUMENT_SYMBOLS:
        case UMI_EDITOR_NAVIGATION_SURFACE_WORKSPACE_SYMBOLS: {
            UmiEditorSymbolNavigationEntry entry;
            UmiStatus status = umi_editor_symbol_navigation_session_selected(
                experience->symbols, &entry);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                *out_location = entry.symbol.selection_location.uri[0] != '\0'
                    ? entry.symbol.selection_location
                    : entry.symbol.location;
            }
            return status;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_NONE:
        default:
            return UMI_STATUS_NOT_FOUND;
    }
}

/*
 * Provide the editor navigation experience active surface operation used by this module
 * and its client applications.
 */
UmiEditorNavigationExperienceSurface umi_editor_navigation_experience_active_surface(
    const UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->active_surface
                              : UMI_EDITOR_NAVIGATION_SURFACE_NONE;
}

/*
 * Provide the editor navigation experience resolution session operation used by this
 * module and its client applications.
 */
UmiEditorNavigationQuerySession *umi_editor_navigation_experience_resolution_session(
    UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->resolution : NULL;
}

/*
 * Provide the editor navigation experience peek session operation used by this module and
 * its client applications.
 */
UmiEditorPeekNavigationSession *umi_editor_navigation_experience_peek_session(
    UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->peek : NULL;
}

/*
 * Provide the editor navigation experience reference session operation used by this module
 * and its client applications.
 */
UmiEditorReferenceNavigationSession *
umi_editor_navigation_experience_reference_session(
    UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->references : NULL;
}

/*
 * Provide the editor navigation experience call hierarchy session operation used by this
 * module and its client applications.
 */
UmiEditorHierarchyNavigationSession *
umi_editor_navigation_experience_call_hierarchy_session(
    UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->call_hierarchy : NULL;
}

/*
 * Provide the editor navigation experience type hierarchy session operation used by this
 * module and its client applications.
 */
UmiEditorHierarchyNavigationSession *
umi_editor_navigation_experience_type_hierarchy_session(
    UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->type_hierarchy : NULL;
}

/*
 * Provide the editor navigation experience symbol session operation used by this module
 * and its client applications.
 */
UmiEditorSymbolNavigationSession *
umi_editor_navigation_experience_symbol_session(
    UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->symbols : NULL;
}

/*
 * Provide the failed reports query operation used by this module and its client
 * applications.
 */
static size_t failed_reports_query(const UmiEditorNavigationQuerySession *session)
{
    UmiEditorNavigationQuerySessionSnapshot snapshot;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_navigation_query_session_snapshot(session, &snapshot) !=
        UMI_STATUS_OK) {
        return 0U;
    }
    return snapshot.failed_provider_count;
}

/*
 * Provide the failed reports hierarchy operation used by this module and its client
 * applications.
 */
static size_t failed_reports_hierarchy(
    const UmiEditorHierarchyNavigationSession *session)
{
    UmiEditorHierarchyNavigationSnapshot snapshot;
    size_t index;
    size_t failed = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_hierarchy_navigation_session_snapshot(session, &snapshot) !=
        UMI_STATUS_OK) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.provider_count; ++index) {
        UmiEditorNavigationProviderReport report;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_hierarchy_navigation_session_provider_report_at(
                session, index, &report) == UMI_STATUS_OK &&
            report.status != UMI_STATUS_OK &&
            report.status != UMI_STATUS_NOT_FOUND) {
            ++failed;
        }
    }
    return failed;
}

/*
 * Provide the failed reports symbols operation used by this module and its client
 * applications.
 */
static size_t failed_reports_symbols(
    const UmiEditorSymbolNavigationSession *session)
{
    UmiEditorSymbolNavigationSnapshot snapshot;
    size_t index;
    size_t failed = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_symbol_navigation_session_snapshot(session, &snapshot) !=
        UMI_STATUS_OK) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.provider_count; ++index) {
        UmiEditorNavigationProviderReport report;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_symbol_navigation_session_provider_report_at(
                session, index, &report) == UMI_STATUS_OK &&
            report.status != UMI_STATUS_OK &&
            report.status != UMI_STATUS_NOT_FOUND) {
            ++failed;
        }
    }
    return failed;
}

/*
 * Provide the editor navigation experience snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_experience_snapshot(
    const UmiEditorNavigationExperience *experience,
    UmiEditorNavigationExperienceSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_NAVIGATION_EXPERIENCE_API_VERSION;
    out_snapshot->active_surface = experience->active_surface;
    out_snapshot->provider_count =
        umi_editor_navigation_provider_registry_count(experience->providers);
    out_snapshot->provider_registry_revision =
        umi_editor_navigation_provider_registry_revision(experience->providers);
    out_snapshot->revision = experience->revision;
    /* Select the behaviour associated with the requested command or state value. */
    switch (experience->active_surface) {
        case UMI_EDITOR_NAVIGATION_SURFACE_RESOLUTION: {
            UmiEditorNavigationQuerySessionSnapshot snapshot;
            (void)umi_editor_navigation_query_session_snapshot(
                experience->resolution, &snapshot);
            out_snapshot->active_item_count = snapshot.result_count;
            out_snapshot->failed_provider_count = snapshot.failed_provider_count;
            out_snapshot->active_session_revision = snapshot.revision;
            out_snapshot->busy =
                snapshot.state == UMI_EDITOR_NAVIGATION_QUERY_SESSION_RUNNING;
            out_snapshot->cancellable = out_snapshot->busy;
            out_snapshot->has_selection = snapshot.has_selection;
            break;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_PEEK: {
            UmiEditorPeekNavigationSnapshot snapshot;
            (void)umi_editor_peek_navigation_session_snapshot(experience->peek,
                                                              &snapshot);
            out_snapshot->active_item_count = snapshot.result_count;
            out_snapshot->failed_provider_count = failed_reports_query(
                umi_editor_peek_navigation_session_query(experience->peek));
            out_snapshot->active_session_revision = snapshot.revision;
            out_snapshot->busy =
                snapshot.state == UMI_EDITOR_PEEK_NAVIGATION_LOADING;
            out_snapshot->cancellable = out_snapshot->busy;
            out_snapshot->has_selection = snapshot.result_count > 0U;
            break;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_REFERENCES: {
            UmiEditorReferenceNavigationSnapshot snapshot;
            (void)umi_editor_reference_navigation_session_snapshot(
                experience->references, &snapshot);
            out_snapshot->active_item_count = snapshot.visible_result_count;
            out_snapshot->failed_provider_count = failed_reports_query(
                umi_editor_reference_navigation_session_query(
                    experience->references));
            out_snapshot->active_session_revision = snapshot.revision;
            out_snapshot->busy =
                snapshot.state == UMI_EDITOR_REFERENCE_NAVIGATION_LOADING;
            out_snapshot->cancellable = out_snapshot->busy;
            out_snapshot->has_selection = snapshot.has_selection;
            break;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY:
        case UMI_EDITOR_NAVIGATION_SURFACE_TYPE_HIERARCHY: {
            UmiEditorHierarchyNavigationSnapshot snapshot;
            const UmiEditorHierarchyNavigationSession *session =
                experience->active_surface ==
                        UMI_EDITOR_NAVIGATION_SURFACE_CALL_HIERARCHY
                    ? experience->call_hierarchy
                    : experience->type_hierarchy;
            (void)umi_editor_hierarchy_navigation_session_snapshot(session,
                                                                   &snapshot);
            out_snapshot->active_item_count = snapshot.visible_node_count;
            out_snapshot->failed_provider_count =
                failed_reports_hierarchy(session);
            out_snapshot->active_session_revision = snapshot.revision;
            out_snapshot->busy =
                snapshot.state == UMI_EDITOR_HIERARCHY_NAVIGATION_LOADING;
            out_snapshot->cancellable = out_snapshot->busy;
            out_snapshot->has_selection = snapshot.has_selection;
            break;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_DOCUMENT_SYMBOLS:
        case UMI_EDITOR_NAVIGATION_SURFACE_WORKSPACE_SYMBOLS: {
            UmiEditorSymbolNavigationSnapshot snapshot;
            (void)umi_editor_symbol_navigation_session_snapshot(
                experience->symbols, &snapshot);
            out_snapshot->active_item_count = snapshot.visible_symbol_count;
            out_snapshot->failed_provider_count = failed_reports_symbols(
                experience->symbols);
            out_snapshot->active_session_revision = snapshot.revision;
            out_snapshot->busy =
                snapshot.state == UMI_EDITOR_SYMBOL_NAVIGATION_LOADING;
            out_snapshot->cancellable = out_snapshot->busy;
            out_snapshot->has_selection = snapshot.has_selection;
            break;
        }
        case UMI_EDITOR_NAVIGATION_SURFACE_NONE:
        default:
            break;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the editor navigation experience revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_navigation_experience_revision(
    const UmiEditorNavigationExperience *experience)
{
    return experience != NULL ? experience->revision : 0U;
}
