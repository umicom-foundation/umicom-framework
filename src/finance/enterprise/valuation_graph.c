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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_graph.h"

#include <string.h>
/* Provide the idx operation used by this module and its client applications. */
static int idx(const UmiEnterpriseValuationGraph *g,const char *id){ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->task_count;++i)/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(g->tasks[i].task_id,id)==0)return(int)i;return-1; }
/* Provide the reach operation used by this module and its client applications. */
static int reach(const UmiEnterpriseValuationGraph *g,const char *a,const char *b,unsigned depth){ size_t i; /* Use the stable identifier comparison to choose the matching record or policy. */ if(depth>64U)return 0; /* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(a,b)==0)return 1; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->dependency_count;++i)/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(g->dependencies[i].prerequisite_task_id,a)==0&&reach(g,g->dependencies[i].dependent_task_id,b,depth+1U))return 1; return 0; }
/*
 * Initialise enterprise valuation graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_valuation_graph_init(UmiEnterpriseValuationGraph *g){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g!=NULL)memset(g,0,sizeof *g); }
/*
 * Provide the enterprise valuation graph add task operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_valuation_graph_add_task(UmiEnterpriseValuationGraph *g,const UmiEnterpriseValuationTask *t){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(idx(g,t->task_id)>=0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->task_count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED; g->tasks[g->task_count++]=*t;return UMI_STATUS_OK; }
/*
 * Provide the enterprise valuation graph add dependency operation used by this module and
 * its client applications.
 */
UmiStatus umi_enterprise_valuation_graph_add_dependency(UmiEnterpriseValuationGraph *g,const UmiEnterpriseValuationDependency *d){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(idx(g,d->prerequisite_task_id)<0||idx(g,d->dependent_task_id)<0)return UMI_STATUS_NOT_FOUND; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(reach(g,d->dependent_task_id,d->prerequisite_task_id,0U))return UMI_STATUS_INVALID_STATE; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->dependency_count>=128U)return UMI_STATUS_CAPACITY_EXCEEDED; g->dependencies[g->dependency_count++]=*d;return UMI_STATUS_OK; }
