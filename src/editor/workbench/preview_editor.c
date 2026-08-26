/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/preview_editor.c
 *
 * PURPOSE:
 *   Track transient preview-editor state and promotion to a permanent tab.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/preview_editor.h"

#include <string.h>
UmiStatus umi_editor_wb_preview_editor_init(UmiEditorWbPreviewEditor *s,const char *id,bool enabled){if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->item_id,sizeof s->item_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->enabled=enabled;s->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_preview_editor_set(UmiEditorWbPreviewEditor *s,bool enabled){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->enabled=enabled;if(enabled)s->promoted=true;s->revision++;return UMI_STATUS_OK;}
int umi_editor_wb_preview_editor_valid(const UmiEditorWbPreviewEditor *s){return s!=NULL&&umi_editor_wb_id_valid(s->item_id)&&s->revision>0U;}
