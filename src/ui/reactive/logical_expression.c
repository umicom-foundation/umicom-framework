/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/logical_expression.c
 *
 * PURPOSE:
 *   Implement boolean logical operands and result.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/logical_expression.h"
/* Evaluate logical operation: 1=AND, 2=OR, 3=XOR. */
UmiStatus umi_ui_reactive_logical_expression_eval(UmiUiReactiveLogicalExpression *item){if(!item)return UMI_STATUS_INVALID_ARGUMENT;switch(item->operation){case 1:item->result=item->left&&item->right;break;case 2:item->result=item->left||item->right;break;case 3:item->result=(item->left!=item->right);break;default:return UMI_STATUS_INVALID_ARGUMENT;}return UMI_STATUS_OK;}
