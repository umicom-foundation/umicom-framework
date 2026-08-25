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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/risk_factor_vector.h"

#include <string.h>
void umi_enterprise_risk_factor_vector_init(UmiEnterpriseRiskFactorVector *v){ if(v!=NULL)memset(v,0,sizeof *v); }
UmiStatus umi_enterprise_risk_factor_vector_add(UmiEnterpriseRiskFactorVector *v,const char *name,double value){ size_t i; if(v==NULL)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<v->count;++i)if(strcmp(v->factors[i].name,name)==0)return UMI_STATUS_ALREADY_EXISTS; if(v->count>=UMI_ENTERPRISE_MAX_FACTORS)return UMI_STATUS_CAPACITY_EXCEEDED; if(umi_enterprise_metric_init(&v->factors[v->count],name,value)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT; v->count++;return UMI_STATUS_OK; }
UmiStatus umi_enterprise_risk_factor_vector_get(const UmiEnterpriseRiskFactorVector *v,const char *name,double *out){ size_t i; if(v==NULL||name==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<v->count;++i)if(strcmp(v->factors[i].name,name)==0){*out=v->factors[i].value;return UMI_STATUS_OK;} return UMI_STATUS_NOT_FOUND; }
