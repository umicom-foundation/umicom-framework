/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editor_intelligence.h
 *
 * PURPOSE:
 *   Coordinate reusable rename, CodeLens, inlay-hint and semantic-highlighting
 *   services without coupling applications to any toolkit or provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor intelligence feature values accepted by this public contract.
 */
typedef enum UmiEditorIntelligenceFeature {
    UMI_EDITOR_INTELLIGENCE_CODE_LENS = 1,
    UMI_EDITOR_INTELLIGENCE_INLAY_HINTS = 2,
    UMI_EDITOR_INTELLIGENCE_SEMANTIC_HIGHLIGHTING = 3
} UmiEditorIntelligenceFeature;

/**
 * Represent the editor intelligence snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor intelligence data shared with callers of this public contract.
 */
typedef struct UmiEditorIntelligence UmiEditorIntelligence;

/**
 * Initialise editor intelligence from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_intelligence_create(
    UmiEditorIntelligence **out_intelligence);
/**
 * Release or reset state held by editor intelligence so the same storage can be reused
 * safely.
 */
void umi_editor_intelligence_destroy(UmiEditorIntelligence *intelligence);
/**
 * Provide the editor intelligence set feature enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intelligence_set_feature_enabled(
    UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceFeature feature,
    int enabled);
/**
 * Provide the editor intelligence feature enabled operation used by this module and its
 * client applications.
 */
int umi_editor_intelligence_feature_enabled(
    const UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceFeature feature);
/**
 * Provide the editor intelligence rename plan operation used by this module and its client
 * applications.
 */
UmiEditorRenameSymbolPlan *umi_editor_intelligence_rename_plan(
    UmiEditorIntelligence *intelligence);
/**
 * Provide the editor intelligence code lenses operation used by this module and its client
 * applications.
 */
UmiEditorCodeLensModel *umi_editor_intelligence_code_lenses(
    UmiEditorIntelligence *intelligence);
/**
 * Provide the editor intelligence inlay hints operation used by this module and its client
 * applications.
 */
UmiEditorInlayHintModel *umi_editor_intelligence_inlay_hints(
    UmiEditorIntelligence *intelligence);
/**
 * Provide the editor intelligence semantic highlighting operation used by this module and
 * its client applications.
 */
UmiEditorSemanticHighlighting *umi_editor_intelligence_semantic_highlighting(
    UmiEditorIntelligence *intelligence);
/**
 * Provide the editor intelligence snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_intelligence_snapshot(
    const UmiEditorIntelligence *intelligence,
    UmiEditorIntelligenceSnapshot *out_snapshot);
/**
 * Provide the editor intelligence revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_intelligence_revision(
    const UmiEditorIntelligence *intelligence);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDITOR_INTELLIGENCE_H */
