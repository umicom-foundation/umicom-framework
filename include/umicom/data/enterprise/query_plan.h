/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_plan.h
 *
 * PURPOSE:
 *   Compose predicate, projection, ordering and join counts into a reviewable backend-neutral query plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_PLAN_H
#define UMICOM_DATA_ENTERPRISE_QUERY_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataQueryPlan { char plan_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; char root_table[UMI_DATA_ENTERPRISE_ID_CAPACITY]; size_t predicate_count; size_t projection_count; size_t join_count; size_t order_count; uint64_t row_limit; bool read_only; } UmiDataQueryPlan;
/* Initialise a query plan rooted at one logical table. */ UmiStatus umi_data_query_plan_init(UmiDataQueryPlan *plan,const char *plan_id,const char *root_table);
/* Add bounded structural complexity evidence. */ UmiStatus umi_data_query_plan_shape(UmiDataQueryPlan *plan,size_t predicates,size_t projections,size_t joins,size_t orders,uint64_t row_limit);
/* Validate safe, bounded query plan state. */ UmiStatus umi_data_query_plan_validate(const UmiDataQueryPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
