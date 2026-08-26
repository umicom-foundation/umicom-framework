/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/conditional_format.c
 *
 * PURPOSE:
 *   Implement renderer-neutral conditional-format evaluation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/conditional_format.h"
int umi_ui_ent_conditional_format_matches(const UmiUiEntConditionalFormat *r,const UmiUiEntCellValue *v){double x;if(!r||!v)return 0;if(v->kind==UMI_UI_ENT_VALUE_INTEGER)x=(double)v->integer_value;else if(v->kind==UMI_UI_ENT_VALUE_REAL)x=v->real_value;else return 0;switch(r->op){case UMI_UI_ENT_COMPARE_LT:return x<r->threshold;case UMI_UI_ENT_COMPARE_LE:return x<=r->threshold;case UMI_UI_ENT_COMPARE_EQ:return x==r->threshold;case UMI_UI_ENT_COMPARE_GE:return x>=r->threshold;case UMI_UI_ENT_COMPARE_GT:return x>r->threshold;default:return 0;}}
