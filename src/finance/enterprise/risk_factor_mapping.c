/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/risk_factor_mapping.c
 *
 * PURPOSE:
 *   Map product identifiers to factor loadings for portfolio risk aggregation.
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

#include "umicom/finance/enterprise/risk_factor_mapping.h"

#include <string.h>
/*
 * Initialise enterprise risk factor mapping from caller-provided values so later
 * operations receive a known state.
 */
void umi_enterprise_risk_factor_mapping_init(UmiEnterpriseRiskFactorMapping *m){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL)memset(m,0,sizeof *m); }
/*
 * Add enterprise risk factor mapping only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_enterprise_risk_factor_mapping_add(UmiEnterpriseRiskFactorMapping *m,const char *pid,const char *factor,double loading){ UmiStatus s; UmiEnterpriseRiskFactorMappingEntry *e; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||!umi_quant_number_valid(loading))return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; e=&m->entries[m->count]; memset(e,0,sizeof *e); s=umi_quant_copy_text(e->product_id.value,sizeof e->product_id.value,pid); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; s=umi_quant_copy_text(e->factor_name,sizeof e->factor_name,factor); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; e->loading=loading;m->count++;return UMI_STATUS_OK; }
/*
 * Provide the enterprise risk factor mapping loading operation used by this module and its
 * client applications.
 */
double umi_enterprise_risk_factor_mapping_loading(const UmiEnterpriseRiskFactorMapping *m,const char *pid,const char *factor){ size_t i; double total=0.0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||pid==NULL||factor==NULL)return 0.0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<m->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(m->entries[i].product_id.value,pid)==0&&strcmp(m->entries[i].factor_name,factor)==0)total+=m->entries[i].loading;return total; }
