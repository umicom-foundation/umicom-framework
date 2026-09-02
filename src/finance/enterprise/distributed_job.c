/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/distributed_job.c
 *
 * PURPOSE:
 *   Represent leaseable distributed work with bounded retry policy.
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

#include "umicom/finance/enterprise/distributed_job.h"

#include <string.h>
/*
 * Initialise enterprise distributed job from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_distributed_job_init(UmiEnterpriseDistributedJob *j,const char *id,const char *kind,int priority,unsigned maxr){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(j==NULL||priority<0||priority>100)return UMI_STATUS_INVALID_ARGUMENT;memset(j,0,sizeof *j);s=umi_quant_copy_text(j->job_id,sizeof j->job_id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_quant_copy_text(j->kind,sizeof j->kind,kind);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;j->priority=priority;j->max_retries=maxr;j->state=UMI_ENTERPRISE_PENDING;return UMI_STATUS_OK;}
/*
 * Provide the enterprise distributed job retry operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_distributed_job_retry(UmiEnterpriseDistributedJob *j){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(j==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(j->retry_count>=j->max_retries)return UMI_STATUS_CAPACITY_EXCEEDED;j->retry_count++;j->state=UMI_ENTERPRISE_PENDING;return UMI_STATUS_OK;}
