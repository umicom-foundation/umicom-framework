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

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the valid feature operation used by this module and its client applications. */
static int valid_feature(UmiEditorIntelligenceFeature feature)
{
    return feature >= UMI_EDITOR_INTELLIGENCE_CODE_LENS &&
           feature <= UMI_EDITOR_INTELLIGENCE_SEMANTIC_HIGHLIGHTING;
}

/*
 * Initialise editor intelligence from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_intelligence_create(
    UmiEditorIntelligence **out_intelligence)
{
    UmiEditorIntelligence *intelligence;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_intelligence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_intelligence = NULL;
    intelligence = (UmiEditorIntelligence *)calloc(1U, sizeof(*intelligence));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (intelligence == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_rename_symbol_plan_create(&intelligence->rename_plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_code_lens_model_create(&intelligence->code_lenses);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_inlay_hint_model_create(&intelligence->inlay_hints);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_semantic_highlighting_create(
            &intelligence->semantic_highlighting);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Release or reset state held by editor intelligence so the same storage can be reused
 * safely.
 */
void umi_editor_intelligence_destroy(UmiEditorIntelligence *intelligence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the editor intelligence set feature enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intelligence_set_feature_enabled(
    UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceFeature feature,
    int enabled)
{
    int value = enabled != 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (intelligence == NULL || !valid_feature(feature)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_INTELLIGENCE_CODE_LENS) {
        intelligence->code_lens_enabled = value;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (feature == UMI_EDITOR_INTELLIGENCE_INLAY_HINTS) {
        intelligence->inlay_hints_enabled = value;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        intelligence->semantic_highlighting_enabled = value;
    }
    intelligence->revision = next_revision(intelligence->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intelligence feature enabled operation used by this module and its
 * client applications.
 */
int umi_editor_intelligence_feature_enabled(
    const UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceFeature feature)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (intelligence == NULL || !valid_feature(feature)) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_INTELLIGENCE_CODE_LENS) {
        return intelligence->code_lens_enabled;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature == UMI_EDITOR_INTELLIGENCE_INLAY_HINTS) {
        return intelligence->inlay_hints_enabled;
    }
    return intelligence->semantic_highlighting_enabled;
}

/*
 * Provide the editor intelligence rename plan operation used by this module and its client
 * applications.
 */
UmiEditorRenameSymbolPlan *umi_editor_intelligence_rename_plan(
    UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->rename_plan : NULL;
}

/*
 * Provide the editor intelligence code lenses operation used by this module and its client
 * applications.
 */
UmiEditorCodeLensModel *umi_editor_intelligence_code_lenses(
    UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->code_lenses : NULL;
}

/*
 * Provide the editor intelligence inlay hints operation used by this module and its client
 * applications.
 */
UmiEditorInlayHintModel *umi_editor_intelligence_inlay_hints(
    UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->inlay_hints : NULL;
}

/*
 * Provide the editor intelligence semantic highlighting operation used by this module and
 * its client applications.
 */
UmiEditorSemanticHighlighting *umi_editor_intelligence_semantic_highlighting(
    UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->semantic_highlighting : NULL;
}

/*
 * Provide the editor intelligence snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_intelligence_snapshot(
    const UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceSnapshot *out_snapshot)
{
    UmiEditorRenameSymbolSnapshot rename_snapshot;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the editor intelligence revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_intelligence_revision(
    const UmiEditorIntelligence *intelligence)
{
    return intelligence != NULL ? intelligence->revision : 0U;
}
