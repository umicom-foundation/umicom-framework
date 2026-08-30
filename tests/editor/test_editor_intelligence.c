/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_intelligence.c
 *
 * PURPOSE:
 *   Implement the test editor intelligence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_intelligence.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/editor_intelligence.h"

int main(void)
{
    UmiEditorIntelligence *intelligence = NULL;
    UmiEditorIntelligenceSnapshot snapshot;
    UmiEditorCodeLensItem lens = {0};

    assert(umi_editor_intelligence_create(&intelligence) == UMI_STATUS_OK);
    assert(umi_editor_intelligence_feature_enabled(
        intelligence, UMI_EDITOR_INTELLIGENCE_CODE_LENS));
    assert(umi_editor_intelligence_set_feature_enabled(
               intelligence, UMI_EDITOR_INTELLIGENCE_INLAY_HINTS, 0) ==
           UMI_STATUS_OK);
    assert(!umi_editor_intelligence_feature_enabled(
        intelligence, UMI_EDITOR_INTELLIGENCE_INLAY_HINTS));

    lens.struct_size = (uint32_t)sizeof(lens);
    lens.api_version = UMI_EDITOR_CODE_LENS_MODEL_API_VERSION;
    (void)strcpy(lens.id, "references");
    (void)strcpy(lens.provider_id, "native-c");
    (void)strcpy(lens.title, "2 references");
    (void)strcpy(lens.command_id, "editor.navigate.references");
    assert(umi_editor_source_location_initialize(
               &lens.location, "file:///workspace/main.c", 1U, 0U) ==
           UMI_STATUS_OK);
    lens.kind = UMI_EDITOR_CODE_LENS_REFERENCE;
    lens.resolution_state = UMI_EDITOR_CODE_LENS_RESOLVED;
    lens.enabled = 1;
    lens.visible = 1;
    assert(umi_editor_code_lens_model_upsert(
               umi_editor_intelligence_code_lenses(intelligence), &lens) ==
           UMI_STATUS_OK);
    assert(umi_editor_intelligence_snapshot(intelligence, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.code_lens_count == 1U);
    assert(!snapshot.inlay_hints_enabled);
    assert(snapshot.semantic_highlighting_enabled);
    umi_editor_intelligence_destroy(intelligence);
    return 0;
}
