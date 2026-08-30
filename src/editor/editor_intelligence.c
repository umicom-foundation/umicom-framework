/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editor_intelligence.c
 *
 * PURPOSE:
 *   Own the reusable editor-intelligence services and publish a consolidated
 *   state snapshot for commands, menus, status bars and future frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/editor_intelligence.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorIntelligence {
    UmiEditorRenameSymbolPlan *rename_plan;
    UmiEditorCodeLensModel *code_lenses;
    UmiEditorInlayHintModel *inlay_hints;
    UmiEditorSemanticHighlighting *semantic_highlighting;
    uint64_t revision;
    int code_lens_enabled;
    int inlay_hints_enabled;
    int semantic_highlighting_enabled;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int valid_feature(UmiEditorIntelligenceFeature feature)
{
    return feature >= UMI_EDITOR_INTELLIGENCE_CODE_LENS &&
           feature <= UMI_EDITOR_INTELLIGENCE_SEMANTIC_HIGHLIGHTING;
}

UmiStatus umi_editor_intelligence_create(
    UmiEditorIntelligence **out_intelligence)
{
    UmiEditorIntelligence *intelligence;
    UmiStatus status;

    if (out_intelligence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_intelligence = NULL;
    intelligence = (UmiEditorIntelligence *)calloc(1U, sizeof(*intelligence));
    if (intelligence == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_rename_symbol_plan_create(&intelligence->rename_plan);
    if (status == UMI_STATUS_OK) {
        status = umi_editor_code_lens_model_create(&intelligence->code_lenses);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_inlay_hint_model_create(&intelligence->inlay_hints);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_editor_semantic_highlighting_create(
            &intelligence->semantic_highlighting);
    }
    if (status != UMI_STATUS_OK) {
        umi_editor_intelligence_destroy(intelligence);
        return status;
    }
    intelligence->code_lens_enabled = 1;
    intelligence->inlay_hints_enabled = 1;
    intelligence->semantic_highlighting_enabled = 1;
    intelligence->revision = 1U;
    *out_intelligence = intelligence;
    return UMI_STATUS_OK;
}

void umi_editor_intelligence_destroy(UmiEditorIntelligence *intelligence)
{
    if (intelligence == NULL) return;
    umi_editor_rename_symbol_plan_destroy(intelligence->rename_plan);
    umi_editor_code_lens_model_destroy(intelligence->code_lenses);
    umi_editor_inlay_hint_model_destroy(intelligence->inlay_hints);
    umi_editor_semantic_highlighting_destroy(
        intelligence->semantic_highlighting);
    intelligence->rename_plan = NULL;
    intelligence->code_lenses = NULL;
    intelligence->inlay_hints = NULL;
    intelligence->semantic_highlighting = NULL;
    free(intelligence);
}

UmiStatus umi_editor_intelligence_set_feature_enabled(
    UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceFeature feature,
    int enabled)
{
    int value = enabled != 0;

    if (intelligence == NULL || !valid_feature(feature)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (feature == UMI_EDITOR_INTELLIGENCE_CODE_LENS) {
        intelligence->code_lens_enabled = value;
    } else if (feature == UMI_EDITOR_INTELLIGENCE_INLAY_HINTS) {
        intelligence->inlay_hints_enabled = value;
    } else {
        intelligence->semantic_highlighting_enabled = value;
    }
    intelligence->revision = next_revision(intelligence->revision);
    return UMI_STATUS_OK;
}

int umi_editor_intelligence_feature_enabled(
    const UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceFeature feature)
{
    if (intelligence == NULL || !valid_feature(feature)) return 0;
    if (feature == UMI_EDITOR_INTELLIGENCE_CODE_LENS) {
        return intelligence->code_lens_enabled;
    }
    if (feature == UMI_EDITOR_INTELLIGENCE_INLAY_HINTS) {
        return intelligence->inlay_hints_enabled;
    }
    return intelligence->semantic_highlighting_enabled;
}

UmiEditorRenameSymbolPlan *umi_editor_intelligence_rename_plan(
    UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->rename_plan : NULL;
}

UmiEditorCodeLensModel *umi_editor_intelligence_code_lenses(
    UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->code_lenses : NULL;
}

UmiEditorInlayHintModel *umi_editor_intelligence_inlay_hints(
    UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->inlay_hints : NULL;
}

UmiEditorSemanticHighlighting *umi_editor_intelligence_semantic_highlighting(
    UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->semantic_highlighting : NULL;
}

UmiStatus umi_editor_intelligence_snapshot(
    const UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceSnapshot *out_snapshot)
{
    UmiEditorRenameSymbolSnapshot rename_snapshot;

    if (intelligence == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_INTELLIGENCE_API_VERSION;
    out_snapshot->code_lens_count =
        umi_editor_code_lens_model_count(intelligence->code_lenses);
    out_snapshot->inlay_hint_count =
        umi_editor_inlay_hint_model_count(intelligence->inlay_hints);
    out_snapshot->semantic_token_count =
        umi_editor_semantic_highlighting_count(
            intelligence->semantic_highlighting);
    if (umi_editor_rename_symbol_plan_snapshot(intelligence->rename_plan,
                                               &rename_snapshot) ==
        UMI_STATUS_OK) {
        out_snapshot->pending_rename_edit_count = rename_snapshot.edit_count;
        out_snapshot->rename_active =
            rename_snapshot.state == UMI_EDITOR_RENAME_SYMBOL_PREPARED ||
            rename_snapshot.state == UMI_EDITOR_RENAME_SYMBOL_APPLYING;
    }
    out_snapshot->revision = intelligence->revision;
    out_snapshot->code_lens_enabled = intelligence->code_lens_enabled;
    out_snapshot->inlay_hints_enabled = intelligence->inlay_hints_enabled;
    out_snapshot->semantic_highlighting_enabled =
        intelligence->semantic_highlighting_enabled;
    return UMI_STATUS_OK;
}

uint64_t umi_editor_intelligence_revision(
    const UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->revision : 0U;
}
