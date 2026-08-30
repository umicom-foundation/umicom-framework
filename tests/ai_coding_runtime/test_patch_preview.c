/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_patch_preview.c
 *
 * PURPOSE:
 *   Verify AI coding runtime patch preview behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/patch_preview.h"

int main(void)
{

    UmiAiCodingPatch patch;
    UmiAiCodingPatchPreview preview;
    assert(umi_ai_coding_patch_init(
        &patch, "patch", "request", "Title", "Reason") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "src/a.c", UMI_AI_CODING_PATCH_CREATE,
        "", "int a;\n") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_preview(&patch, &preview) == UMI_STATUS_OK);
    assert(preview.file_count == 1U);
    assert(preview.create_count == 1U);

    return 0;
}
