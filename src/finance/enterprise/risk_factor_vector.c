/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/risk_factor_vector.c
 *
 * PURPOSE:
 *   Store bounded named risk-factor observations with deterministic lookup.
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

#include "umicom/finance/enterprise/risk_factor_vector.h"

#include <string.h>
/*
 * Initialise enterprise risk factor vector from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_risk_factor_vector_init(UmiEnterpriseRiskFactorVector *v){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v!=NULL)memset(v,0,sizeof *v); }
/*
 * Add enterprise risk factor vector only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_risk_factor_vector_add(UmiEnterpriseRiskFactorVector *v,const char *name,double value){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<v->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(v->factors[i].name,name)==0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v->count>=UMI_ENTERPRISE_MAX_FACTORS)return UMI_STATUS_CAPACITY_EXCEEDED; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_enterprise_metric_init(&v->factors[v->count],name,value)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT; v->count++;return UMI_STATUS_OK; }
/*
 * Provide the enterprise risk factor vector get operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_risk_factor_vector_get(const UmiEnterpriseRiskFactorVector *v,const char *name,double *out){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||name==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<v->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(v->factors[i].name,name)==0){*out=v->factors[i].value;return UMI_STATUS_OK;} return UMI_STATUS_NOT_FOUND; }
