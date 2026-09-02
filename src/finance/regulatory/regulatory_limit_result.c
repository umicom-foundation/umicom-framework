/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/regulatory_limit_result.c
 *
 * PURPOSE:
 *   Evaluate observed regulatory metrics against minimum or maximum limits.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/regulatory_limit_result.h"

#include <string.h>
/*
 * Provide the reg regulatory limit result evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_reg_regulatory_limit_result_evaluate(UmiRegulatoryLimitResult *r,const char *id,double observed,double threshold,int minimum_rule){ UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||!umi_reg_number_valid(observed)||!umi_reg_number_valid(threshold)||(minimum_rule!=0&&minimum_rule!=1))return UMI_STATUS_INVALID_ARGUMENT;memset(r,0,sizeof *r);s=umi_reg_copy_text(r->limit_id,sizeof r->limit_id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;r->observed=observed;r->threshold=threshold;r->breached=minimum_rule?observed<threshold:observed>threshold;return UMI_STATUS_OK; }
