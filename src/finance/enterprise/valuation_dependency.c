/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_dependency.c
 *
 * PURPOSE:
 *   Describe ordering dependencies between valuation tasks.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_dependency.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_dependency_init(UmiEnterpriseValuationDependency *d,const char *a,const char *b){ UmiStatus s; if(d==NULL||a==NULL||b==NULL||strcmp(a,b)==0)return UMI_STATUS_INVALID_ARGUMENT; memset(d,0,sizeof *d); s=umi_quant_copy_text(d->prerequisite_task_id,sizeof d->prerequisite_task_id,a); if(s!=UMI_STATUS_OK)return s; return umi_quant_copy_text(d->dependent_task_id,sizeof d->dependent_task_id,b); }
