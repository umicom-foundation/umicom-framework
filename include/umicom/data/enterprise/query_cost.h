/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_cost.h
 *
 * PURPOSE:
 *   Estimate deterministic relative query cost for policy and routing decisions without backend execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_COST_H
#define UMICOM_DATA_ENTERPRISE_QUERY_COST_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/query_plan.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data query cost data shared with callers of this public contract.
 */
typedef struct UmiDataQueryCost { double estimated_cost; double join_penalty; double predicate_discount; bool expensive; } UmiDataQueryCost;
/* Estimate a portable relative query cost from query-plan structure. */ UmiStatus umi_data_query_cost_estimate(const UmiDataQueryPlan *plan,double expensive_threshold,UmiDataQueryCost *out_cost);

#ifdef __cplusplus
}
#endif
#endif
