/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editor_intelligence.h
 *
 * PURPOSE:
 *   Coordinate reusable rename, CodeLens, inlay-hint and semantic-highlighting
 *   services without coupling applications to any toolkit or provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_EDITOR_INTELLIGENCE_H
#define UMICOM_EDITOR_EDITOR_INTELLIGENCE_H

#include "umicom/editor/code_lens_model.h"
#include "umicom/editor/inlay_hint_model.h"
#include "umicom/editor/rename_symbol.h"
#include "umicom/editor/semantic_highlighting.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTELLIGENCE_API_VERSION 1U

typedef enum UmiEditorIntelligenceFeature {
    UMI_EDITOR_INTELLIGENCE_CODE_LENS = 1,
    UMI_EDITOR_INTELLIGENCE_INLAY_HINTS = 2,
    UMI_EDITOR_INTELLIGENCE_SEMANTIC_HIGHLIGHTING = 3
} UmiEditorIntelligenceFeature;

typedef struct UmiEditorIntelligenceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t code_lens_count;
    size_t inlay_hint_count;
    size_t semantic_token_count;
    size_t pending_rename_edit_count;
    uint64_t revision;
    int code_lens_enabled;
    int inlay_hints_enabled;
    int semantic_highlighting_enabled;
    int rename_active;
} UmiEditorIntelligenceSnapshot;

typedef struct UmiEditorIntelligence UmiEditorIntelligence;

UmiStatus umi_editor_intelligence_create(
    UmiEditorIntelligence **out_intelligence);
void umi_editor_intelligence_destroy(UmiEditorIntelligence *intelligence);
UmiStatus umi_editor_intelligence_set_feature_enabled(
    UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceFeature feature,
    int enabled);
int umi_editor_intelligence_feature_enabled(
    const UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceFeature feature);
UmiEditorRenameSymbolPlan *umi_editor_intelligence_rename_plan(
    UmiEditorIntelligence *intelligence);
UmiEditorCodeLensModel *umi_editor_intelligence_code_lenses(
    UmiEditorIntelligence *intelligence);
UmiEditorInlayHintModel *umi_editor_intelligence_inlay_hints(
    UmiEditorIntelligence *intelligence);
UmiEditorSemanticHighlighting *umi_editor_intelligence_semantic_highlighting(
    UmiEditorIntelligence *intelligence);
UmiStatus umi_editor_intelligence_snapshot(
    const UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceSnapshot *out_snapshot);
uint64_t umi_editor_intelligence_revision(
    const UmiEditorIntelligence *intelligence);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDITOR_INTELLIGENCE_H */
