/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_status_model.c
 *
 * PURPOSE:
 *   Expose line, column, selection, language, encoding and dirty status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_status_model.h"

#include <string.h>
UmiStatus umi_editor_wb_editor_status_model_init(UmiEditorWbEditorStatusModel *s,const char *lang,const char *enc){if(s==NULL||lang==NULL||enc==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->language,sizeof s->language,lang)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->encoding,sizeof s->encoding,enc)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;(void)umi_editor_wb_copy_text(s->eol,sizeof s->eol,"LF");s->line=1U;s->column=1U;return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_editor_status_model_set_position(UmiEditorWbEditorStatusModel *s,uint32_t line,uint32_t col){if(s==NULL||line==0U||col==0U)return UMI_STATUS_INVALID_ARGUMENT;s->line=line;s->column=col;return UMI_STATUS_OK;}
