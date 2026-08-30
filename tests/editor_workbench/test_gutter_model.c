/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_gutter_model.c
 *
 * PURPOSE:
 *   Implement the test gutter model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/gutter_model.h"
int main(void){ UmiEditorWbGutterModel s; umi_editor_wb_gutter_model_init(&s); if(umi_editor_wb_gutter_model_channel_count(&s)!=4U)return 1; return 0; }
