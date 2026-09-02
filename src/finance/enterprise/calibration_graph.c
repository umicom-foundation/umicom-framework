/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/calibration_graph.c
 *
 * PURPOSE:
 *   Maintain a bounded acyclic dependency graph for calibration workflows.
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

#include "umicom/finance/enterprise/calibration_graph.h"

#include <string.h>
/* Provide the index of operation used by this module and its client applications. */
static int index_of(const UmiEnterpriseCalibrationGraph *g,const char *id){ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->job_count;++i)/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(g->jobs[i].job_id,id)==0)return (int)i; return -1; }
/* Provide the reaches operation used by this module and its client applications. */
static int reaches(const UmiEnterpriseCalibrationGraph *g,const char *from,const char *to,unsigned depth){ size_t i; /* Use the stable identifier comparison to choose the matching record or policy. */ if(depth>32U)return 0; /* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(from,to)==0)return 1; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->dependency_count;++i)/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(g->dependencies[i].prerequisite_id,from)==0&&reaches(g,g->dependencies[i].dependent_id,to,depth+1U))return 1; return 0; }
/*
 * Initialise enterprise calibration graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_calibration_graph_init(UmiEnterpriseCalibrationGraph *g){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g!=NULL)memset(g,0,sizeof *g); }
/*
 * Provide the enterprise calibration graph add job operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_calibration_graph_add_job(UmiEnterpriseCalibrationGraph *g,const UmiEnterpriseCalibrationJob *j){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||j==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(index_of(g,j->job_id)>=0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->job_count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED; g->jobs[g->job_count++]=*j; return UMI_STATUS_OK; }
/*
 * Provide the enterprise calibration graph add dependency operation used by this module
 * and its client applications.
 */
UmiStatus umi_enterprise_calibration_graph_add_dependency(UmiEnterpriseCalibrationGraph *g,const UmiEnterpriseCalibrationDependency *d){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(index_of(g,d->prerequisite_id)<0||index_of(g,d->dependent_id)<0)return UMI_STATUS_NOT_FOUND; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(reaches(g,d->dependent_id,d->prerequisite_id,0U))return UMI_STATUS_INVALID_STATE; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->dependency_count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED; g->dependencies[g->dependency_count++]=*d; return UMI_STATUS_OK; }
