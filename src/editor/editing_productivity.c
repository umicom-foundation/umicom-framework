/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editing_productivity.c
 *
 * PURPOSE:
 *   Own the reusable editing-productivity services and expose a single stable
 *   coordination boundary to lightweight application shells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/editing_productivity.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorEditingProductivity {
    UmiEditorFormattingProfileRegistry *formatting_profiles;
    UmiEditorFormattingSession *formatting_session;
    UmiEditorImportOrganisationPlan *import_organisation;
    UmiEditorSnippetSession *snippet_session;
    UmiEditorLinkedEditingModel *linked_editing;
    uint64_t revision;
    int formatting_enabled;
    int import_organisation_enabled;
    int snippets_enabled;
    int linked_editing_enabled;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the valid feature operation used by this module and its client applications. */
static int valid_feature(UmiEditorEditingProductivityFeature feature)
{
    return feature >= UMI_EDITOR_PRODUCTIVITY_FORMATTING &&
           feature <= UMI_EDITOR_PRODUCTIVITY_LINKED_EDITING;
}

/* Provide the add revision operation used by this module and its client applications. */
static uint64_t add_revision(uint64_t left, uint64_t right)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (UINT64_MAX - left < right) {
        uint64_t mixed = left ^ right;
        return mixed == UINT64_MAX ? 1U : mixed + 1U;
    }
    return left + right;
}

/*
 * Provide the composite revision operation used by this module and its client
 * applications.
 */
static uint64_t composite_revision(
    const UmiEditorEditingProductivity *productivity)
{
    uint64_t revision;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (productivity == NULL) return 0U;
    revision = productivity->revision;
    revision = add_revision(
        revision,
        umi_editor_formatting_profile_registry_revision(
            productivity->formatting_profiles));
    revision = add_revision(
        revision,
        umi_editor_formatting_session_revision(
            productivity->formatting_session));
    revision = add_revision(
        revision,
        umi_editor_import_organisation_plan_revision(
            productivity->import_organisation));
    revision = add_revision(
        revision,
        umi_editor_snippet_session_revision(productivity->snippet_session));
    return add_revision(
        revision,
        umi_editor_linked_editing_model_revision(productivity->linked_editing));
}

/*
 * Initialise editor editing productivity from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_editing_productivity_create(
    UmiEditorEditingProductivity **out_productivity)
{
    UmiEditorEditingProductivity *productivity;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_productivity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_productivity = NULL;
    productivity = (UmiEditorEditingProductivity *)calloc(
        1U, sizeof(*productivity));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (productivity == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_formatting_profile_registry_create(
        &productivity->formatting_profiles);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_formatting_session_create(
            &productivity->formatting_session);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_import_organisation_plan_create(
            &productivity->import_organisation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_snippet_session_create(
            &productivity->snippet_session);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_linked_editing_model_create(
            &productivity->linked_editing);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_editor_editing_productivity_destroy(productivity);
        return status;
    }
    productivity->formatting_enabled = 1;
    productivity->import_organisation_enabled = 1;
    productivity->snippets_enabled = 1;
    productivity->linked_editing_enabled = 1;
    productivity->revision = 1U;
    *out_productivity = productivity;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor editing productivity so the same storage can be
 * reused safely.
 */
void umi_editor_editing_productivity_destroy(
    UmiEditorEditingProductivity *productivity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (productivity == NULL) return;
    umi_editor_formatting_profile_registry_destroy(
        productivity->formatting_profiles);
    umi_editor_formatting_session_destroy(productivity->formatting_session);
    umi_editor_import_organisation_plan_destroy(
        productivity->import_organisation);
    umi_editor_snippet_session_destroy(productivity->snippet_session);
    umi_editor_linked_editing_model_destroy(productivity->linked_editing);
    productivity->formatting_profiles = NULL;
    productivity->formatting_session = NULL;
    productivity->import_organisation = NULL;
    productivity->snippet_session = NULL;
    productivity->linked_editing = NULL;
    free(productivity);
}

/*
 * Provide the editor editing productivity set feature enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_editing_productivity_set_feature_enabled(
    UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivityFeature feature,
    int enabled)
{
    int value = enabled != 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (productivity == NULL || !valid_feature(feature)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_PRODUCTIVITY_FORMATTING) {
        productivity->formatting_enabled = value;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (feature == UMI_EDITOR_PRODUCTIVITY_IMPORT_ORGANISATION) {
        productivity->import_organisation_enabled = value;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (feature == UMI_EDITOR_PRODUCTIVITY_SNIPPETS) {
        productivity->snippets_enabled = value;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        productivity->linked_editing_enabled = value;
    }
    productivity->revision = next_revision(productivity->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor editing productivity feature enabled operation used by this module
 * and its client applications.
 */
int umi_editor_editing_productivity_feature_enabled(
    const UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivityFeature feature)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (productivity == NULL || !valid_feature(feature)) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_PRODUCTIVITY_FORMATTING) {
        return productivity->formatting_enabled;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_PRODUCTIVITY_IMPORT_ORGANISATION) {
        return productivity->import_organisation_enabled;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_PRODUCTIVITY_SNIPPETS) {
        return productivity->snippets_enabled;
    }
    return productivity->linked_editing_enabled;
}

/*
 * Provide the editor editing productivity formatting profiles operation used by this
 * module and its client applications.
 */
UmiEditorFormattingProfileRegistry *
umi_editor_editing_productivity_formatting_profiles(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->formatting_profiles : NULL;
}

/*
 * Provide the editor editing productivity formatting session operation used by this module
 * and its client applications.
 */
UmiEditorFormattingSession *umi_editor_editing_productivity_formatting_session(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->formatting_session : NULL;
}

/*
 * Provide the editor editing productivity import organisation operation used by this
 * module and its client applications.
 */
UmiEditorImportOrganisationPlan *
umi_editor_editing_productivity_import_organisation(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->import_organisation : NULL;
}

/*
 * Provide the editor editing productivity snippet session operation used by this module
 * and its client applications.
 */
UmiEditorSnippetSession *umi_editor_editing_productivity_snippet_session(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->snippet_session : NULL;
}

/*
 * Provide the editor editing productivity linked editing operation used by this module and
 * its client applications.
 */
UmiEditorLinkedEditingModel *umi_editor_editing_productivity_linked_editing(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->linked_editing : NULL;
}

/*
 * Provide the editor editing productivity snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_editing_productivity_snapshot(
    const UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivitySnapshot *out_snapshot)
{
    UmiEditorFormattingSessionSnapshot formatting;
    UmiEditorImportOrganisationSnapshot imports;
    UmiEditorSnippetSessionSnapshot snippet;
    UmiEditorLinkedEditingSnapshot linked;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (productivity == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_formatting_session_snapshot(
        productivity->formatting_session, &formatting);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_import_organisation_plan_snapshot(
        productivity->import_organisation, &imports);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_snippet_session_snapshot(
        productivity->snippet_session, &snippet);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_linked_editing_model_snapshot(
        productivity->linked_editing, &linked);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_EDITING_PRODUCTIVITY_API_VERSION;
    out_snapshot->formatting_profile_count =
        umi_editor_formatting_profile_registry_count(
            productivity->formatting_profiles);
    out_snapshot->import_count = imports.import_count;
    out_snapshot->snippet_placeholder_count = snippet.placeholder_count;
    out_snapshot->linked_range_count = linked.range_count;
    out_snapshot->formatting_state = formatting.state;
    out_snapshot->snippet_state = snippet.state;
    out_snapshot->revision = composite_revision(productivity);
    out_snapshot->formatting_enabled = productivity->formatting_enabled;
    out_snapshot->import_organisation_enabled =
        productivity->import_organisation_enabled;
    out_snapshot->snippets_enabled = productivity->snippets_enabled;
    out_snapshot->linked_editing_enabled = productivity->linked_editing_enabled;
    out_snapshot->import_plan_analysed = imports.analysed;
    out_snapshot->linked_model_finalized = linked.finalized;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor editing productivity revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_editing_productivity_revision(
    const UmiEditorEditingProductivity *productivity)
{
    return composite_revision(productivity);
}
