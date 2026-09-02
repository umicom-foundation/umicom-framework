/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/risk_limit_evaluation.c
 *
 * PURPOSE:
 *   Evaluate observed risk against warning and hard limit thresholds.
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

#include "umicom/finance/enterprise/risk_limit_evaluation.h"

#include <math.h>
#include <string.h>
/*
 * Provide the enterprise risk limit evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_enterprise_risk_limit_evaluate(UmiEnterpriseRiskLimitEvaluation *e,const char *id,double warn,double hard,double obs){UmiStatus s;double a;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||!umi_quant_number_valid(warn)||!umi_quant_number_valid(hard)||!umi_quant_number_valid(obs)||warn<0.0||hard<=0.0||warn>hard)return UMI_STATUS_INVALID_ARGUMENT;memset(e,0,sizeof *e);s=umi_quant_copy_text(e->limit_id,sizeof e->limit_id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;a=fabs(obs);e->warning_limit=warn;e->hard_limit=hard;e->observed=obs;e->utilisation=a/hard;e->warning=(a>=warn)?1:0;e->breach=(a>hard)?1:0;return UMI_STATUS_OK;}
