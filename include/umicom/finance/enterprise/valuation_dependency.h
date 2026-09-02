/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_dependency.h
 *
 * PURPOSE:
 *   Describe ordering dependencies between valuation tasks.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_DEPENDENCY_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_DEPENDENCY_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise valuation dependency data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationDependency { char prerequisite_task_id[UMI_ENTERPRISE_ID_CAPACITY]; char dependent_task_id[UMI_ENTERPRISE_ID_CAPACITY]; } UmiEnterpriseValuationDependency;
/* Initialise a directed valuation-task dependency while rejecting self-dependency. */
UmiStatus umi_enterprise_valuation_dependency_init(UmiEnterpriseValuationDependency *dependency,const char *prerequisite_task_id,const char *dependent_task_id);

#ifdef __cplusplus
}
#endif

#endif
