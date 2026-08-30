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
UmiNativeConstantValue umi_nc_constant_signed(int64_t v){UmiNativeConstantValue x;memset(&x,0,sizeof(x));x.kind=UMI_NC_CONST_SIGNED;x.signed_value=v;return x;}
UmiNativeConstantValue umi_nc_constant_unsigned(uint64_t v){UmiNativeConstantValue x;memset(&x,0,sizeof(x));x.kind=UMI_NC_CONST_UNSIGNED;x.unsigned_value=v;return x;}
UmiNativeConstantValue umi_nc_constant_bool(bool v){UmiNativeConstantValue x;memset(&x,0,sizeof(x));x.kind=UMI_NC_CONST_BOOL;x.bool_value=v;return x;}
bool umi_nc_constant_truthy(const UmiNativeConstantValue *v){if(v==NULL)return false;switch(v->kind){case UMI_NC_CONST_SIGNED:return v->signed_value!=0;case UMI_NC_CONST_UNSIGNED:return v->unsigned_value!=0U;case UMI_NC_CONST_FLOAT:return v->float_value!=0.0;case UMI_NC_CONST_BOOL:return v->bool_value;default:return false;}}
bool umi_nc_constant_equal(const UmiNativeConstantValue *a,const UmiNativeConstantValue *b){if(a==NULL||b==NULL||a->kind!=b->kind)return false;switch(a->kind){case UMI_NC_CONST_SIGNED:return a->signed_value==b->signed_value;case UMI_NC_CONST_UNSIGNED:return a->unsigned_value==b->unsigned_value;case UMI_NC_CONST_FLOAT:return a->float_value==b->float_value;case UMI_NC_CONST_BOOL:return a->bool_value==b->bool_value;default:return false;}}
