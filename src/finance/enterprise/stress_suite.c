/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/stress_suite.c
 *
 * PURPOSE:
 *   Maintain a bounded stress-scenario suite and expose its highest severity.
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

#include "umicom/finance/enterprise/stress_suite.h"

#include <string.h>
/*
 * Initialise enterprise stress suite from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_stress_suite_init(UmiEnterpriseStressSuite *s,const char *id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);return umi_quant_copy_text(s->suite_id,sizeof s->suite_id,id);}
/*
 * Add enterprise stress suite only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_stress_suite_add(UmiEnterpriseStressSuite *s,const UmiEnterpriseStressScenario *x){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||x==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->scenarios[i].scenario_id,x->scenario_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED;s->scenarios[s->count++]=*x;return UMI_STATUS_OK;}
/*
 * Provide the enterprise stress suite max severity operation used by this module and its
 * client applications.
 */
int umi_enterprise_stress_suite_max_severity(const UmiEnterpriseStressSuite *s){size_t i;int m=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->scenarios[i].severity>m)m=s->scenarios[i].severity;return m;}
