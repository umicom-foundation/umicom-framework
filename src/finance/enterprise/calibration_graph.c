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
static int index_of(const UmiEnterpriseCalibrationGraph *g,const char *id){ size_t i; for(i=0U;i<g->job_count;++i)if(strcmp(g->jobs[i].job_id,id)==0)return (int)i; return -1; }
static int reaches(const UmiEnterpriseCalibrationGraph *g,const char *from,const char *to,unsigned depth){ size_t i; if(depth>32U)return 0; if(strcmp(from,to)==0)return 1; for(i=0U;i<g->dependency_count;++i)if(strcmp(g->dependencies[i].prerequisite_id,from)==0&&reaches(g,g->dependencies[i].dependent_id,to,depth+1U))return 1; return 0; }
void umi_enterprise_calibration_graph_init(UmiEnterpriseCalibrationGraph *g){ if(g!=NULL)memset(g,0,sizeof *g); }
UmiStatus umi_enterprise_calibration_graph_add_job(UmiEnterpriseCalibrationGraph *g,const UmiEnterpriseCalibrationJob *j){ if(g==NULL||j==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(index_of(g,j->job_id)>=0)return UMI_STATUS_ALREADY_EXISTS; if(g->job_count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED; g->jobs[g->job_count++]=*j; return UMI_STATUS_OK; }
UmiStatus umi_enterprise_calibration_graph_add_dependency(UmiEnterpriseCalibrationGraph *g,const UmiEnterpriseCalibrationDependency *d){ if(g==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(index_of(g,d->prerequisite_id)<0||index_of(g,d->dependent_id)<0)return UMI_STATUS_NOT_FOUND; if(reaches(g,d->dependent_id,d->prerequisite_id,0U))return UMI_STATUS_INVALID_STATE; if(g->dependency_count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED; g->dependencies[g->dependency_count++]=*d; return UMI_STATUS_OK; }
