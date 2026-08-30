/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_preview_editor.c
 *
 * PURPOSE:
 *   Implement the test preview editor behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/preview_editor.h"
int main(void){ UmiEditorWbPreviewEditor s; if(umi_editor_wb_preview_editor_init(&s,"item",false)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_preview_editor_set(&s,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_preview_editor_valid(&s)||!s.enabled)return 3; return 0; }
