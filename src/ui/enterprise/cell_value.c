/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/cell_value.c
 *
 * PURPOSE:
 *   Implement typed cell values and equality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_value.h"
#include <string.h>
/*
 * Release or reset state held by ui ent cell value so the same storage can be reused
 * safely.
 */
void umi_ui_ent_cell_value_clear(UmiUiEntCellValue *v){/* Apply this branch only when its contract condition is satisfied. */ if(v){memset(v,0,sizeof *v);v->kind=UMI_UI_ENT_VALUE_EMPTY;}}
/*
 * Provide the ui ent cell value set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_cell_value_set_text(UmiUiEntCellValue *v,const char *t){UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!v)return UMI_STATUS_INVALID_ARGUMENT;umi_ui_ent_cell_value_clear(v);s=umi_ui_ent_copy_text(v->text,sizeof v->text,t);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK)v->kind=UMI_UI_ENT_VALUE_TEXT;return s;}
/*
 * Provide the ui ent cell value set integer operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_integer(UmiUiEntCellValue *v,int64_t n){/* Apply this branch only when its contract condition is satisfied. */ if(v){umi_ui_ent_cell_value_clear(v);v->kind=UMI_UI_ENT_VALUE_INTEGER;v->integer_value=n;}}
/*
 * Provide the ui ent cell value set real operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_real(UmiUiEntCellValue *v,double n){/* Apply this branch only when its contract condition is satisfied. */ if(v){umi_ui_ent_cell_value_clear(v);v->kind=UMI_UI_ENT_VALUE_REAL;v->real_value=n;}}
/*
 * Provide the ui ent cell value set boolean operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_boolean(UmiUiEntCellValue *v,int e){/* Apply this branch only when its contract condition is satisfied. */ if(v){umi_ui_ent_cell_value_clear(v);v->kind=UMI_UI_ENT_VALUE_BOOLEAN;v->boolean_value=e?1:0;}}
/*
 * Provide the ui ent cell value equal operation used by this module and its client
 * applications.
 */
int umi_ui_ent_cell_value_equal(const UmiUiEntCellValue *a,const UmiUiEntCellValue *b){/* Use the stable identifier comparison to choose the matching record or policy. */ if(!a||!b||a->kind!=b->kind)return 0;/* Select the behaviour associated with the requested command or state value. */ switch(a->kind){case UMI_UI_ENT_VALUE_EMPTY:return 1;case UMI_UI_ENT_VALUE_TEXT:return strcmp(a->text,b->text)==0;case UMI_UI_ENT_VALUE_INTEGER:return a->integer_value==b->integer_value;case UMI_UI_ENT_VALUE_REAL:return a->real_value==b->real_value;case UMI_UI_ENT_VALUE_BOOLEAN:return a->boolean_value==b->boolean_value;default:return 0;}}
