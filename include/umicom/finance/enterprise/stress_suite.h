/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/stress_suite.h
 *
 * PURPOSE:
 *   Maintain a bounded stress-scenario suite and expose its highest severity.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_STRESS_SUITE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_STRESS_SUITE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/stress_scenario.h"
/**
 * Represent the enterprise stress suite data shared with callers of this public contract.
 */
typedef struct UmiEnterpriseStressSuite { char suite_id[UMI_ENTERPRISE_ID_CAPACITY]; UmiEnterpriseStressScenario scenarios[32U]; size_t count; } UmiEnterpriseStressSuite;
/* Initialise an empty stress suite. */
UmiStatus umi_enterprise_stress_suite_init(UmiEnterpriseStressSuite *suite,const char *suite_id);
/* Add a unique stress scenario. */
UmiStatus umi_enterprise_stress_suite_add(UmiEnterpriseStressSuite *suite,const UmiEnterpriseStressScenario *scenario);
/* Return the maximum severity represented in the suite. */
int umi_enterprise_stress_suite_max_severity(const UmiEnterpriseStressSuite *suite);

#ifdef __cplusplus
}
#endif

#endif
