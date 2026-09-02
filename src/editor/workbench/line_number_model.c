/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/line_number_model.c
 *
 * PURPOSE:
 *   Resolve absolute, relative or hidden line-number labels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/line_number_model.h"

/*
 * Initialise editor wb line number model from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_line_number_model_init(UmiEditorWbLineNumberModel *m,UmiEditorWbLineNumberMode mode,uint32_t cursor){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL){m->mode=mode;m->cursor_line=cursor;}}
/*
 * Provide the editor wb line number model value operation used by this module and its
 * client applications.
 */
uint32_t umi_editor_wb_line_number_model_value(const UmiEditorWbLineNumberModel *m,uint32_t line){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||m->mode==UMI_EDITOR_WB_LINE_NUMBERS_HIDDEN)return 0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->mode==UMI_EDITOR_WB_LINE_NUMBERS_ABSOLUTE||line==m->cursor_line)return line;return line>m->cursor_line?line-m->cursor_line:m->cursor_line-line;}
