/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/repository_plan.c
 *
 * PURPOSE:
 *   Build explicit ORM persistence operations from tracked changes before any SQL is executed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/repository_plan.h"
#include <string.h>

/* Plan reset supports repeatable preview/rebuild workflows. */ void umi_data_repository_plan_init(UmiDataRepositoryPlan *plan){if(plan!=NULL)(void)memset(plan,0,sizeof(*plan));}
/* Each tracked change becomes an explicit reviewable persistence operation. */ UmiStatus umi_data_repository_plan_build(UmiDataRepositoryPlan *plan,const UmiDataChangeTracker *tracker){size_t i;UmiStatus s;if(plan==NULL||tracker==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_data_repository_plan_init(plan);if(tracker->count>UMI_DATA_ENTERPRISE_MAX_OPERATIONS)return UMI_STATUS_CAPACITY_EXCEEDED;for(i=0;i<tracker->count;++i){UmiDataRepositoryOperation *op=&plan->items[plan->count];s=umi_data_enterprise_copy_text(op->operation_id,sizeof(op->operation_id),tracker->items[i].change_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(op->entity_id,sizeof(op->entity_id),tracker->items[i].entity_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(op->key,sizeof(op->key),tracker->items[i].key);if(s!=UMI_STATUS_OK)return s;op->kind=tracker->items[i].kind;op->order=(uint32_t)i;plan->count++;}return UMI_STATUS_OK;}
/* Validation guards deterministic replay and auditability. */ UmiStatus umi_data_repository_plan_validate(const UmiDataRepositoryPlan *plan){size_t i;if(plan==NULL||plan->count==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<plan->count;++i)if(plan->items[i].operation_id[0]=='\0'||plan->items[i].order!=(uint32_t)i)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
