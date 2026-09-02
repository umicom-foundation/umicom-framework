/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/constant_eval.c
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
#include "umicom/compiler/native/constant_eval.h"
/* Provide the as i64 operation used by this module and its client applications. */
static UmiStatus as_i64(const UmiNativeConstantValue *v,int64_t *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v->kind==UMI_NC_CONST_SIGNED){*out=v->signed_value;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v->kind==UMI_NC_CONST_BOOL){*out=v->bool_value?1:0;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v->kind==UMI_NC_CONST_UNSIGNED&&v->unsigned_value<=(uint64_t)INT64_MAX){*out=(int64_t)v->unsigned_value;return UMI_STATUS_OK;}return UMI_STATUS_INVALID_ARGUMENT;}
/*
 * Provide the nc constant eval binary operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_constant_eval_binary(UmiNativeConstOp op,const UmiNativeConstantValue *l,const UmiNativeConstantValue *r,UmiNativeConstantValue *out){int64_t a=0,b=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiStatus sa=as_i64(l,&a),sb=as_i64(r,&b);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(sa!=UMI_STATUS_OK||sb!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Select the behaviour associated with the requested command or state value. */ switch(op){case UMI_NC_CONST_ADD:*out=umi_nc_constant_signed(a+b);break;case UMI_NC_CONST_SUB:*out=umi_nc_constant_signed(a-b);break;case UMI_NC_CONST_MUL:*out=umi_nc_constant_signed(a*b);break;case UMI_NC_CONST_DIV:/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==0)return UMI_STATUS_INVALID_STATE;*out=umi_nc_constant_signed(a/b);break;case UMI_NC_CONST_MOD:/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==0)return UMI_STATUS_INVALID_STATE;*out=umi_nc_constant_signed(a%b);break;case UMI_NC_CONST_AND:*out=umi_nc_constant_signed(a&b);break;case UMI_NC_CONST_OR:*out=umi_nc_constant_signed(a|b);break;case UMI_NC_CONST_XOR:*out=umi_nc_constant_signed(a^b);break;case UMI_NC_CONST_SHL:/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b<0||b>=63)return UMI_STATUS_INVALID_ARGUMENT;*out=umi_nc_constant_signed(a<<b);break;case UMI_NC_CONST_SHR:/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b<0||b>=63)return UMI_STATUS_INVALID_ARGUMENT;*out=umi_nc_constant_signed(a>>b);break;case UMI_NC_CONST_EQ:*out=umi_nc_constant_bool(a==b);break;case UMI_NC_CONST_NE:*out=umi_nc_constant_bool(a!=b);break;case UMI_NC_CONST_LT:*out=umi_nc_constant_bool(a<b);break;case UMI_NC_CONST_LE:*out=umi_nc_constant_bool(a<=b);break;case UMI_NC_CONST_GT:*out=umi_nc_constant_bool(a>b);break;case UMI_NC_CONST_GE:*out=umi_nc_constant_bool(a>=b);break;default:return UMI_STATUS_NOT_FOUND;}return UMI_STATUS_OK;}
/*
 * Provide the nc constant eval not operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_constant_eval_not(const UmiNativeConstantValue *v,UmiNativeConstantValue *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=umi_nc_constant_bool(!umi_nc_constant_truthy(v));return UMI_STATUS_OK;}
