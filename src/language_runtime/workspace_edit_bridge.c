/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/workspace_edit_bridge.c
 *
 * PURPOSE:
 *   Project LSP rename/refactor edits into the established Editor validation/apply pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/workspace_edit_bridge.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_language_runtime_workspace_edit_to_editor(const UmiLanguageRuntimeWorkspaceEdit*r,UmiEditorWorkspaceEditSet**out){UmiEditorWorkspaceEditSet*s=NULL;size_t i;UmiStatus q;if(!r||!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;q=umi_editor_workspace_edit_set_create(&s);if(q!=UMI_STATUS_OK)return q;for(i=0;i<r->count;i++){UmiEditorWorkspaceTextEdit x={0};if(strlen(r->items[i].edit.new_text)>=sizeof(x.replacement_text)){q=UMI_STATUS_CAPACITY_EXCEEDED;goto fail;}x.struct_size=sizeof(x);x.api_version=UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;snprintf(x.id,sizeof(x.id),"lsp.edit.%zu",i);snprintf(x.provider_id,sizeof(x.provider_id),"lsp");q=umi_editor_source_location_initialize(&x.location,r->items[i].uri,r->items[i].edit.range.start.line,r->items[i].edit.range.start.character);if(q!=UMI_STATUS_OK)goto fail;x.location.kind=UMI_EDITOR_SOURCE_LOCATION_EDIT;x.location.end_line=r->items[i].edit.range.end.line;x.location.end_column=r->items[i].edit.range.end.character;snprintf(x.replacement_text,sizeof(x.replacement_text),"%s",r->items[i].edit.new_text);x.state=UMI_EDITOR_WORKSPACE_EDIT_READY;x.required=1;q=umi_editor_workspace_edit_set_upsert(s,&x);if(q!=UMI_STATUS_OK)goto fail;}q=umi_editor_workspace_edit_set_finalize(s);if(q!=UMI_STATUS_OK)goto fail;*out=s;return UMI_STATUS_OK;fail:umi_editor_workspace_edit_set_destroy(s);return q;}
