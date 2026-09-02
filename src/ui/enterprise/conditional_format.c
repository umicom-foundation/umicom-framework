/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/conditional_format.c
 *
 * PURPOSE:
 *   Implement renderer-neutral conditional-format evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/conditional_format.h"
/*
 * Provide the ui ent conditional format matches operation used by this module and its
 * client applications.
 */
int umi_ui_ent_conditional_format_matches(const UmiUiEntConditionalFormat *r,const UmiUiEntCellValue *v){double x;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!r||!v)return 0;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v->kind==UMI_UI_ENT_VALUE_INTEGER)x=(double)v->integer_value;else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v->kind==UMI_UI_ENT_VALUE_REAL)x=v->real_value;/* Use this fallback path when the earlier condition does not apply. */ else return 0;/* Select the behaviour associated with the requested command or state value. */ switch(r->op){case UMI_UI_ENT_COMPARE_LT:return x<r->threshold;case UMI_UI_ENT_COMPARE_LE:return x<=r->threshold;case UMI_UI_ENT_COMPARE_EQ:return x==r->threshold;case UMI_UI_ENT_COMPARE_GE:return x>=r->threshold;case UMI_UI_ENT_COMPARE_GT:return x>r->threshold;default:return 0;}}
