/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/constant_value.c
 *
 * PURPOSE:
 *   Represent compile-time integer, floating-point and boolean constants without host pointer aliasing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/constant_value.h"
#include <string.h>
/*
 * Provide the nc constant signed operation used by this module and its client
 * applications.
 */
UmiNativeConstantValue umi_nc_constant_signed(int64_t v){UmiNativeConstantValue x;memset(&x,0,sizeof(x));x.kind=UMI_NC_CONST_SIGNED;x.signed_value=v;return x;}
/*
 * Provide the nc constant unsigned operation used by this module and its client
 * applications.
 */
UmiNativeConstantValue umi_nc_constant_unsigned(uint64_t v){UmiNativeConstantValue x;memset(&x,0,sizeof(x));x.kind=UMI_NC_CONST_UNSIGNED;x.unsigned_value=v;return x;}
/* Provide the nc constant bool operation used by this module and its client applications. */
UmiNativeConstantValue umi_nc_constant_bool(bool v){UmiNativeConstantValue x;memset(&x,0,sizeof(x));x.kind=UMI_NC_CONST_BOOL;x.bool_value=v;return x;}
/*
 * Provide the nc constant truthy operation used by this module and its client
 * applications.
 */
bool umi_nc_constant_truthy(const UmiNativeConstantValue *v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL)return false;/* Select the behaviour associated with the requested command or state value. */ switch(v->kind){case UMI_NC_CONST_SIGNED:return v->signed_value!=0;case UMI_NC_CONST_UNSIGNED:return v->unsigned_value!=0U;case UMI_NC_CONST_FLOAT:return v->float_value!=0.0;case UMI_NC_CONST_BOOL:return v->bool_value;default:return false;}}
/* Provide the nc constant equal operation used by this module and its client applications. */
bool umi_nc_constant_equal(const UmiNativeConstantValue *a,const UmiNativeConstantValue *b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||b==NULL||a->kind!=b->kind)return false;/* Select the behaviour associated with the requested command or state value. */ switch(a->kind){case UMI_NC_CONST_SIGNED:return a->signed_value==b->signed_value;case UMI_NC_CONST_UNSIGNED:return a->unsigned_value==b->unsigned_value;case UMI_NC_CONST_FLOAT:return a->float_value==b->float_value;case UMI_NC_CONST_BOOL:return a->bool_value==b->bool_value;default:return false;}}
