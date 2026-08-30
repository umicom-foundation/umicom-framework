/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/constant_eval.h
 *
 * PURPOSE:
 *   Evaluate a safe integer subset of C constant expressions for folding and conditional compilation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_CONSTANT_EVAL_H
#define UMICOM_COMPILER_NATIVE_CONSTANT_EVAL_H
#include "umicom/base/status.h"
#include "umicom/compiler/native/constant_value.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiNativeConstOp { UMI_NC_CONST_ADD=1, UMI_NC_CONST_SUB=2, UMI_NC_CONST_MUL=3, UMI_NC_CONST_DIV=4, UMI_NC_CONST_MOD=5, UMI_NC_CONST_AND=6, UMI_NC_CONST_OR=7, UMI_NC_CONST_XOR=8, UMI_NC_CONST_SHL=9, UMI_NC_CONST_SHR=10, UMI_NC_CONST_EQ=11, UMI_NC_CONST_NE=12, UMI_NC_CONST_LT=13, UMI_NC_CONST_LE=14, UMI_NC_CONST_GT=15, UMI_NC_CONST_GE=16 } UmiNativeConstOp;
UmiStatus umi_nc_constant_eval_binary(UmiNativeConstOp op,const UmiNativeConstantValue *left,const UmiNativeConstantValue *right,UmiNativeConstantValue *out_value);
UmiStatus umi_nc_constant_eval_not(const UmiNativeConstantValue *value,UmiNativeConstantValue *out_value);
#ifdef __cplusplus
}
#endif
#endif
