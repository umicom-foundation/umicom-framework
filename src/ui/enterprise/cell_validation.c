/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/cell_validation.c
 *
 * PURPOSE:
 *   Implement reusable cell validation rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_validation.h"
#include <string.h>
/* Provide the number operation used by this module and its client applications. */
static double number(const UmiUiEntCellValue *v,int *ok){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(v->kind==UMI_UI_ENT_VALUE_INTEGER){*ok=1;return (double)v->integer_value;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(v->kind==UMI_UI_ENT_VALUE_REAL){*ok=1;return v->real_value;}*ok=0;return 0.0;}
/*
 * Check that ui ent cell validation satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ui_ent_cell_validation_validate(const UmiUiEntCellValidationRule *r,const UmiUiEntCellValue *v,char *m,size_t cap){int ok;double n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!r||!v||!m||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;m[0]='\0';/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r->required&&(v->kind==UMI_UI_ENT_VALUE_EMPTY||(v->kind==UMI_UI_ENT_VALUE_TEXT&&v->text[0]=='\0'))){umi_ui_ent_copy_text(m,cap,"value is required");return UMI_STATUS_INVALID_ARGUMENT;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v->kind==UMI_UI_ENT_VALUE_TEXT&&r->max_length>0U&&strlen(v->text)>r->max_length){umi_ui_ent_copy_text(m,cap,"text is too long");return UMI_STATUS_INVALID_ARGUMENT;}n=number(v,&ok);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(ok&&r->has_minimum&&n<r->minimum){umi_ui_ent_copy_text(m,cap,"value is below minimum");return UMI_STATUS_INVALID_ARGUMENT;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(ok&&r->has_maximum&&n>r->maximum){umi_ui_ent_copy_text(m,cap,"value is above maximum");return UMI_STATUS_INVALID_ARGUMENT;}return UMI_STATUS_OK;}
