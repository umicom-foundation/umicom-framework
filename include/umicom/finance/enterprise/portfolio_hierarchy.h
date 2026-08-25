/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/portfolio_hierarchy.h
 *
 * PURPOSE:
 *   Maintain an acyclic parent-child hierarchy for portfolio risk aggregation.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_HIERARCHY_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PORTFOLIO_HIERARCHY_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterprisePortfolioHierarchyNode { char node_id[UMI_ENTERPRISE_ID_CAPACITY]; char parent_id[UMI_ENTERPRISE_ID_CAPACITY]; double weight; } UmiEnterprisePortfolioHierarchyNode;
typedef struct UmiEnterprisePortfolioHierarchy { UmiEnterprisePortfolioHierarchyNode nodes[UMI_ENTERPRISE_MAX_HIERARCHY_NODES]; size_t count; } UmiEnterprisePortfolioHierarchy;
/* Initialise an empty hierarchy. */
void umi_enterprise_portfolio_hierarchy_init(UmiEnterprisePortfolioHierarchy *hierarchy);
/* Add one node while rejecting duplicate identifiers and ancestry cycles. */
UmiStatus umi_enterprise_portfolio_hierarchy_add(UmiEnterprisePortfolioHierarchy *hierarchy,const UmiEnterprisePortfolioHierarchyNode *node);

#ifdef __cplusplus
}
#endif

#endif
