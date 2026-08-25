/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_graph.c
 *
 * PURPOSE:
 *   Maintain an acyclic dependency graph for valuation task execution.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_graph.h"

#include <string.h>
static int idx(const UmiEnterpriseValuationGraph *g,const char *id){ size_t i; for(i=0U;i<g->task_count;++i)if(strcmp(g->tasks[i].task_id,id)==0)return(int)i;return-1; }
static int reach(const UmiEnterpriseValuationGraph *g,const char *a,const char *b,unsigned depth){ size_t i; if(depth>64U)return 0; if(strcmp(a,b)==0)return 1; for(i=0U;i<g->dependency_count;++i)if(strcmp(g->dependencies[i].prerequisite_task_id,a)==0&&reach(g,g->dependencies[i].dependent_task_id,b,depth+1U))return 1; return 0; }
void umi_enterprise_valuation_graph_init(UmiEnterpriseValuationGraph *g){ if(g!=NULL)memset(g,0,sizeof *g); }
UmiStatus umi_enterprise_valuation_graph_add_task(UmiEnterpriseValuationGraph *g,const UmiEnterpriseValuationTask *t){ if(g==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(idx(g,t->task_id)>=0)return UMI_STATUS_ALREADY_EXISTS; if(g->task_count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED; g->tasks[g->task_count++]=*t;return UMI_STATUS_OK; }
UmiStatus umi_enterprise_valuation_graph_add_dependency(UmiEnterpriseValuationGraph *g,const UmiEnterpriseValuationDependency *d){ if(g==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(idx(g,d->prerequisite_task_id)<0||idx(g,d->dependent_task_id)<0)return UMI_STATUS_NOT_FOUND; if(reach(g,d->dependent_task_id,d->prerequisite_task_id,0U))return UMI_STATUS_INVALID_STATE; if(g->dependency_count>=128U)return UMI_STATUS_CAPACITY_EXCEEDED; g->dependencies[g->dependency_count++]=*d;return UMI_STATUS_OK; }
