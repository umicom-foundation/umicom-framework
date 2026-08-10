/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_editor_perspective.c
 *
 * PURPOSE:
 *   Verify editor cursor state and named workbench perspectives.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
    UmiUiEditorModel *editors = NULL;
    UmiUiPerspectiveModel *perspectives = NULL;
    UmiUiEditorSnapshot editor = {0};
    UmiUiPerspectiveSnapshot perspective = {0};
    assert(umi_ui_editor_model_create(&editors) == UMI_STATUS_OK);
    assert(umi_ui_perspective_model_create(&perspectives) == UMI_STATUS_OK);
    (void)snprintf(editor.editor_id, sizeof(editor.editor_id), "%s", "editor.main");
    (void)snprintf(editor.document_id, sizeof(editor.document_id), "%s", "doc.main");
    editor.cursor_offset = 42;
    assert(umi_ui_editor_model_upsert(editors, &editor) == UMI_STATUS_OK);
    (void)snprintf(perspective.perspective_id, sizeof(perspective.perspective_id), "%s", "studio.develop");
    (void)snprintf(perspective.title, sizeof(perspective.title), "%s", "Develop");
    perspective.active = 1;
    assert(umi_ui_perspective_model_upsert(perspectives, &perspective) == UMI_STATUS_OK);
    umi_ui_perspective_model_destroy(perspectives);
    umi_ui_editor_model_destroy(editors);
    return EXIT_SUCCESS;
}
