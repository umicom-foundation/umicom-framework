/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/types.c
 *
 * PURPOSE:
 *   Implement safe shared primitives for advanced trading services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/types.h"
#include <limits.h>
#include <string.h>
/* Copy bounded text and reject truncation. */
UmiStatus umi_trading_core_copy_text(char *destination,size_t capacity,const char *source) { if(destination==NULL||source==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT; size_t n=strlen(source); if(n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED; memcpy(destination,source,n+1U); return UMI_STATUS_OK; }
/* Assign a bounded Framework financial identifier. */
UmiStatus umi_trading_core_id_assign(UmiFinancialId *id,const char *text) { if(id==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(id,0,sizeof *id); return umi_trading_core_copy_text(id->value,sizeof id->value,text); }
/* Compare two Framework financial identifiers. */
bool umi_trading_core_id_equal(const UmiFinancialId *left,const UmiFinancialId *right) { return left!=NULL&&right!=NULL&&strcmp(left->value,right->value)==0; }
/* Return an absolute signed value while protecting INT64_MIN. */
UmiStatus umi_trading_core_abs_i64(int64_t value,int64_t *out_value) { if(out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(value==INT64_MIN)return UMI_STATUS_CAPACITY_EXCEEDED; *out_value=value<0?-value:value; return UMI_STATUS_OK; }
/* Add signed 64-bit values with overflow detection. */
UmiStatus umi_trading_core_add_i64(int64_t left,int64_t right,int64_t *out_value) { if(out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT; if((right>0&&left>INT64_MAX-right)||(right<0&&left<INT64_MIN-right))return UMI_STATUS_CAPACITY_EXCEEDED; *out_value=left+right; return UMI_STATUS_OK; }
