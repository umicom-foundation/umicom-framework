/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_ai_editor_bridge.c
 *
 * PURPOSE:
 *   Integration regression coverage for AI editor request projection, including
 *   compatibility runtime identity and the explicit runtime-aware API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/ide_integration/ai_editor_bridge.h"

int main(void)
{
    UmiIdeEditorSelection selection;
    UmiAiCodingRequest request;

    assert(umi_ide_editor_selection_set(
        &selection,
        "doc",
        "src/a.c",
        "c",
        "int a;",
        1U,
        0U,
        1U,
        6U,
        2U,
        0) == UMI_STATUS_OK);

    /* Existing callers remain valid and receive a deterministic runtime id. */
    assert(umi_ide_ai_request_from_selection(
        &selection,
        UMI_AI_CODING_TASK_EXPLAIN,
        "req",
        "session",
        ".",
        "Explain",
        &request) == UMI_STATUS_OK);
    assert(strcmp(request.active_path, "src/a.c") == 0);
    assert(strcmp(request.runtime_id, "session") == 0);
    assert(request.selection_start_line == 1U);

    /* New callers can identify the actual runtime independently of session. */
    assert(umi_ide_ai_request_from_selection_runtime(
        &selection,
        UMI_AI_CODING_TASK_REFACTOR,
        "req.2",
        "session.2",
        "studio.local.runtime",
        ".",
        "Refactor",
        &request) == UMI_STATUS_OK);
    assert(strcmp(request.runtime_id, "studio.local.runtime") == 0);
    assert(strcmp(request.session_id, "session.2") == 0);

    return 0;
}
