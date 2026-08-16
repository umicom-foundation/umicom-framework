/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editing_productivity.c
 *
 * PURPOSE:
 *   Own the reusable editing-productivity services and expose a single stable
 *   coordination boundary to lightweight application shells.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int valid_feature(UmiEditorEditingProductivityFeature feature)
{
    return feature >= UMI_EDITOR_PRODUCTIVITY_FORMATTING &&
           feature <= UMI_EDITOR_PRODUCTIVITY_LINKED_EDITING;
}

static uint64_t add_revision(uint64_t left, uint64_t right)
{
    if (UINT64_MAX - left < right) {
        uint64_t mixed = left ^ right;
        return mixed == UINT64_MAX ? 1U : mixed + 1U;
    }
    return left + right;
}

static uint64_t composite_revision(
    const UmiEditorEditingProductivity *productivity)
{
    uint64_t revision;

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

UmiStatus umi_editor_editing_productivity_create(
    UmiEditorEditingProductivity **out_productivity)
{
    UmiEditorEditingProductivity *productivity;
    UmiStatus status;

    if (out_productivity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_productivity = NULL;
    productivity = (UmiEditorEditingProductivity *)calloc(
        1U, sizeof(*productivity));
    if (productivity == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_formatting_profile_registry_create(
        &productivity->formatting_profiles);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_formatting_session_create(
            &productivity->formatting_session);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_import_organisation_plan_create(
            &productivity->import_organisation);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_snippet_session_create(
            &productivity->snippet_session);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_linked_editing_model_create(
            &productivity->linked_editing);
    }
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

void umi_editor_editing_productivity_destroy(
    UmiEditorEditingProductivity *productivity)
{
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

UmiStatus umi_editor_editing_productivity_set_feature_enabled(
    UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivityFeature feature,
    int enabled)
{
    int value = enabled != 0;

    if (productivity == NULL || !valid_feature(feature)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (feature == UMI_EDITOR_PRODUCTIVITY_FORMATTING) {
        productivity->formatting_enabled = value;
    } else if (feature == UMI_EDITOR_PRODUCTIVITY_IMPORT_ORGANISATION) {
        productivity->import_organisation_enabled = value;
    } else if (feature == UMI_EDITOR_PRODUCTIVITY_SNIPPETS) {
        productivity->snippets_enabled = value;
    } else {
        productivity->linked_editing_enabled = value;
    }
    productivity->revision = next_revision(productivity->revision);
    return UMI_STATUS_OK;
}

int umi_editor_editing_productivity_feature_enabled(
    const UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivityFeature feature)
{
    if (productivity == NULL || !valid_feature(feature)) return 0;
    if (feature == UMI_EDITOR_PRODUCTIVITY_FORMATTING) {
        return productivity->formatting_enabled;
    }
    if (feature == UMI_EDITOR_PRODUCTIVITY_IMPORT_ORGANISATION) {
        return productivity->import_organisation_enabled;
    }
    if (feature == UMI_EDITOR_PRODUCTIVITY_SNIPPETS) {
        return productivity->snippets_enabled;
    }
    return productivity->linked_editing_enabled;
}

UmiEditorFormattingProfileRegistry *
umi_editor_editing_productivity_formatting_profiles(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->formatting_profiles : NULL;
}

UmiEditorFormattingSession *umi_editor_editing_productivity_formatting_session(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->formatting_session : NULL;
}

UmiEditorImportOrganisationPlan *
umi_editor_editing_productivity_import_organisation(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->import_organisation : NULL;
}

UmiEditorSnippetSession *umi_editor_editing_productivity_snippet_session(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->snippet_session : NULL;
}

UmiEditorLinkedEditingModel *umi_editor_editing_productivity_linked_editing(
    UmiEditorEditingProductivity *productivity)
{
    return productivity != NULL ? productivity->linked_editing : NULL;
}

UmiStatus umi_editor_editing_productivity_snapshot(
    const UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivitySnapshot *out_snapshot)
{
    UmiEditorFormattingSessionSnapshot formatting;
    UmiEditorImportOrganisationSnapshot imports;
    UmiEditorSnippetSessionSnapshot snippet;
    UmiEditorLinkedEditingSnapshot linked;
    UmiStatus status;

    if (productivity == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_formatting_session_snapshot(
        productivity->formatting_session, &formatting);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_import_organisation_plan_snapshot(
        productivity->import_organisation, &imports);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_snippet_session_snapshot(
        productivity->snippet_session, &snippet);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_linked_editing_model_snapshot(
        productivity->linked_editing, &linked);
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

uint64_t umi_editor_editing_productivity_revision(
    const UmiEditorEditingProductivity *productivity)
{
    return composite_revision(productivity);
}
