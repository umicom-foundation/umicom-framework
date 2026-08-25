/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_plan.c
 *
 * PURPOSE:
 *   Compose predicate, projection, ordering and join counts into a reviewable backend-neutral query plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_plan.h"
#include <string.h>

/* Initialisation keeps query semantics independent from SQL-string construction. */ UmiStatus umi_data_query_plan_init(UmiDataQueryPlan *plan,const char *plan_id,const char *root_table){UmiStatus s;if(plan==NULL||plan_id==NULL||root_table==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(plan,0,sizeof(*plan));s=umi_data_enterprise_copy_text(plan->plan_id,sizeof(plan->plan_id),plan_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(plan->root_table,sizeof(plan->root_table),root_table);if(s!=UMI_STATUS_OK)return s;plan->read_only=true;return UMI_STATUS_OK;}
/* Shape evidence feeds cost, policy and observability without executing a query. */ UmiStatus umi_data_query_plan_shape(UmiDataQueryPlan *plan,size_t predicates,size_t projections,size_t joins,size_t orders,uint64_t row_limit){if(plan==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(predicates>UMI_DATA_ENTERPRISE_MAX_ITEMS||projections>UMI_DATA_ENTERPRISE_MAX_ITEMS||joins>UMI_DATA_ENTERPRISE_MAX_ITEMS||orders>UMI_DATA_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;plan->predicate_count=predicates;plan->projection_count=projections;plan->join_count=joins;plan->order_count=orders;plan->row_limit=row_limit;return umi_data_query_plan_validate(plan);}
/* Validation blocks accidental unbounded reads by requiring an explicit row limit. */ UmiStatus umi_data_query_plan_validate(const UmiDataQueryPlan *plan){if(plan==NULL||plan->plan_id[0]=='\0'||plan->root_table[0]=='\0'||plan->row_limit==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
