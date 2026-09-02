/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editing_productivity.h
 *
 * PURPOSE:
 *   Coordinate Framework-owned formatting, import organisation, snippet and
 *   linked-editing services for Studio and every future Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor editing productivity feature values accepted by this public
 * contract.
 */
typedef enum UmiEditorEditingProductivityFeature {
    UMI_EDITOR_PRODUCTIVITY_FORMATTING = 1,
    UMI_EDITOR_PRODUCTIVITY_IMPORT_ORGANISATION = 2,
    UMI_EDITOR_PRODUCTIVITY_SNIPPETS = 3,
    UMI_EDITOR_PRODUCTIVITY_LINKED_EDITING = 4
} UmiEditorEditingProductivityFeature;

/**
 * Represent the editor editing productivity snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor editing productivity data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorEditingProductivity UmiEditorEditingProductivity;

/**
 * Initialise editor editing productivity from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_editing_productivity_create(
    UmiEditorEditingProductivity **out_productivity);
/**
 * Release or reset state held by editor editing productivity so the same storage can be
 * reused safely.
 */
void umi_editor_editing_productivity_destroy(
    UmiEditorEditingProductivity *productivity);
/**
 * Provide the editor editing productivity set feature enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_editing_productivity_set_feature_enabled(
    UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivityFeature feature,
    int enabled);
/**
 * Provide the editor editing productivity feature enabled operation used by this module
 * and its client applications.
 */
int umi_editor_editing_productivity_feature_enabled(
    const UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivityFeature feature);
/**
 * Provide the editor editing productivity formatting profiles operation used by this
 * module and its client applications.
 */
UmiEditorFormattingProfileRegistry *
umi_editor_editing_productivity_formatting_profiles(
    UmiEditorEditingProductivity *productivity);
/**
 * Provide the editor editing productivity formatting session operation used by this module
 * and its client applications.
 */
UmiEditorFormattingSession *umi_editor_editing_productivity_formatting_session(
    UmiEditorEditingProductivity *productivity);
/**
 * Provide the editor editing productivity import organisation operation used by this
 * module and its client applications.
 */
UmiEditorImportOrganisationPlan *
umi_editor_editing_productivity_import_organisation(
    UmiEditorEditingProductivity *productivity);
/**
 * Provide the editor editing productivity snippet session operation used by this module
 * and its client applications.
 */
UmiEditorSnippetSession *umi_editor_editing_productivity_snippet_session(
    UmiEditorEditingProductivity *productivity);
/**
 * Provide the editor editing productivity linked editing operation used by this module and
 * its client applications.
 */
UmiEditorLinkedEditingModel *umi_editor_editing_productivity_linked_editing(
    UmiEditorEditingProductivity *productivity);
/**
 * Provide the editor editing productivity snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_editing_productivity_snapshot(
    const UmiEditorEditingProductivity *productivity,
    UmiEditorEditingProductivitySnapshot *out_snapshot);
/**
 * Provide the editor editing productivity revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_editing_productivity_revision(
    const UmiEditorEditingProductivity *productivity);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDITING_PRODUCTIVITY_H */
