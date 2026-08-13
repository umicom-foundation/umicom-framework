/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/memory_budget.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Memory Budget developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_MEMORY_BUDGET_H
#define UMICOM_DEVELOPER_CAPABILITIES_MEMORY_BUDGET_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_MEMORY_BUDGET "umicom.developer.performance.memory_budget"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_memory_budget(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_MEMORY_BUDGET_H */
