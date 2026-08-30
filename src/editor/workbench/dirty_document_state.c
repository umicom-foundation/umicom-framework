/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/dirty_document_state.c
 *
 * PURPOSE:
 *   Track dirty and saved revisions for an editor document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/dirty_document_state.h"

#include <string.h>
UmiStatus umi_editor_wb_dirty_document_state_init(UmiEditorWbDirtyDocumentState *s,const char *id,bool enabled){if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->item_id,sizeof s->item_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->enabled=enabled;s->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_dirty_document_state_set(UmiEditorWbDirtyDocumentState *s,bool enabled){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->enabled=enabled;if(enabled)s->promoted=true;s->revision++;return UMI_STATUS_OK;}
int umi_editor_wb_dirty_document_state_valid(const UmiEditorWbDirtyDocumentState *s){return s!=NULL&&umi_editor_wb_id_valid(s->item_id)&&s->revision>0U;}
