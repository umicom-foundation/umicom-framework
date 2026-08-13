/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/parallel_job_budget.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Parallel Job Budget developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_JOB_BUDGET_H
#define UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_JOB_BUDGET_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PARALLEL_JOB_BUDGET "umicom.developer.performance.parallel_job_budget"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_parallel_job_budget(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_JOB_BUDGET_H */
