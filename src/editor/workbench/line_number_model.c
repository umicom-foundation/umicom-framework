/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/line_number_model.c
 *
 * PURPOSE:
 *   Resolve absolute, relative or hidden line-number labels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/line_number_model.h"

void umi_editor_wb_line_number_model_init(UmiEditorWbLineNumberModel *m,UmiEditorWbLineNumberMode mode,uint32_t cursor){if(m!=NULL){m->mode=mode;m->cursor_line=cursor;}}
uint32_t umi_editor_wb_line_number_model_value(const UmiEditorWbLineNumberModel *m,uint32_t line){if(m==NULL||m->mode==UMI_EDITOR_WB_LINE_NUMBERS_HIDDEN)return 0U;if(m->mode==UMI_EDITOR_WB_LINE_NUMBERS_ABSOLUTE||line==m->cursor_line)return line;return line>m->cursor_line?line-m->cursor_line:m->cursor_line-line;}
