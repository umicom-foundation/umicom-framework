/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_workspace_edit_bridge.c
 *
 * PURPOSE:
 *   Verify LSP WorkspaceEdit projects into the existing Editor edit-set contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/workspace_edit_bridge.h"
int main(void){UmiLanguageRuntimeWorkspaceEdit r={0};UmiEditorWorkspaceEditSet*s=NULL;UmiEditorWorkspaceEditSnapshot snap;strcpy(r.items[0].uri,"file:///a.c");r.items[0].edit.range.start.line=1;r.items[0].edit.range.end.line=1;r.items[0].edit.range.end.character=1;strcpy(r.items[0].edit.new_text,"x");r.count=1;assert(umi_language_runtime_workspace_edit_to_editor(&r,&s)==UMI_STATUS_OK);assert(umi_editor_workspace_edit_set_snapshot(s,&snap)==UMI_STATUS_OK);assert(snap.edit_count==1);umi_editor_workspace_edit_set_destroy(s);return 0;}
