/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editing_productivity.h
 *
 * PURPOSE:
 *   Coordinate Framework-owned formatting, import organisation, snippet and
 *   linked-editing services for Studio and every future Umicom application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_EDITING_PRODUCTIVITY_H
#define UMICOM_EDITOR_EDITING_PRODUCTIVITY_H

#include "umicom/editor/formatting_profile.h"
#include "umicom/editor/formatting_session.h"
#include "umicom/editor/import_organisation.h"
#include "umicom/editor/linked_editing_model.h"
#include "umicom/editor/snippet_session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_EDITING_PRODUCTIVITY_API_VERSION 1U

typedef enum UmiEditorEditingProductivityFeature {
    UMI_EDITOR_PRODUCTIVITY_FORMATTING = 1,
    UMI_EDITOR_PRODUCTIVITY_IMPORT_ORGANISATION = 2,
    UMI_EDITOR_PRODUCTIVITY_SNIPPETS = 3,
    UMI_EDITOR_PRODUCTIVITY_LINKED_EDITING = 4
} UmiEditorEditingProductivityFeature;

typedef struct UmiEditorEditingProductivitySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t formatting_profile_count;
    size_t import_count;
    size_t snippet_placeholder_count;
    size_t linked_range_count;
    UmiEditorFormattingSessionState formatting_state;
    UmiEditorSnippetSessionState snippet_state;
    uint64_t revision;
    int formatting_enabled;
    int import_organisation_enabled;
    int snippets_enabled;
    int linked_editing_enabled;
    int import_plan_analysed;
    int linked_model_finalized;
} UmiEditorEditingProductivitySnapshot;

typedef struct UmiEditorEditingProductivity UmiEditorEditingProductivity;

UmiStatus umi_editor_editing_productivity_create(
    UmiEditorEditingProductivity **out_productivity);
void umi_editor_editing_productivity_destroy(
    UmiEditorEditingProductivity *productivity);
UmiStatus umi_editor_editing_productivity_set_feature_enabled(
    UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivityFeature feature,
    int enabled);
int umi_editor_editing_productivity_feature_enabled(
    const UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivityFeature feature);
UmiEditorFormattingProfileRegistry *
umi_editor_editing_productivity_formatting_profiles(
    UmiEditorEditingProductivity *productivity);
UmiEditorFormattingSession *umi_editor_editing_productivity_formatting_session(
    UmiEditorEditingProductivity *productivity);
UmiEditorImportOrganisationPlan *
umi_editor_editing_productivity_import_organisation(
    UmiEditorEditingProductivity *productivity);
UmiEditorSnippetSession *umi_editor_editing_productivity_snippet_session(
    UmiEditorEditingProductivity *productivity);
UmiEditorLinkedEditingModel *umi_editor_editing_productivity_linked_editing(
    UmiEditorEditingProductivity *productivity);
UmiStatus umi_editor_editing_productivity_snapshot(
    const UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivitySnapshot *out_snapshot);
uint64_t umi_editor_editing_productivity_revision(
    const UmiEditorEditingProductivity *productivity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDITING_PRODUCTIVITY_H */
