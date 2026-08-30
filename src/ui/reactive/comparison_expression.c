/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/comparison_expression.c
 *
 * PURPOSE:
 *   Implement numeric comparison operands and result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/comparison_expression.h"
/* Evaluate numeric comparison: -2 <=, -1 <, 0 ==, 1 >, 2 >=, 3 !=. */
UmiStatus umi_ui_reactive_comparison_expression_eval(UmiUiReactiveComparisonExpression *item){if(!item)return UMI_STATUS_INVALID_ARGUMENT;switch(item->comparison){case -2:item->result=item->left<=item->right;break;case -1:item->result=item->left<item->right;break;case 0:item->result=item->left==item->right;break;case 1:item->result=item->left>item->right;break;case 2:item->result=item->left>=item->right;break;case 3:item->result=item->left!=item->right;break;default:return UMI_STATUS_INVALID_ARGUMENT;}return UMI_STATUS_OK;}
