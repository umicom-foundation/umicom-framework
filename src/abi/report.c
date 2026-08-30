/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/report.c
 *
 * PURPOSE:
 *   Format ABI compatibility results for humans and automation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/report.h"
#include <stdio.h>
UmiStatus umi_abi_report_format(const UmiAbiCompatibility *r,char *b,size_t cap){int n;if(r==NULL||b==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(b,cap,"compatible=%s missing_features=%zu missing_symbols=%zu incompatible_layouts=%zu",r->compatible?"yes":"no",r->missing_features,r->missing_symbols,r->incompatible_layouts);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
