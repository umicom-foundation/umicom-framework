/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_replace_session.c
 *
 * PURPOSE:
 *   Implement the test editor replace session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_replace_session.h"
int main(void){ UmiEditorWbEditorReplaceSession s; if(umi_editor_wb_editor_replace_session_init(&s,"id","text")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_replace_session_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_editor_replace_session_valid(&s)||s.primary!=3U)return 3; return 0; }
