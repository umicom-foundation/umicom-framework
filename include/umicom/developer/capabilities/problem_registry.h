/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/problem_registry.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Problem Registry developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PROBLEM_REGISTRY_H
#define UMICOM_DEVELOPER_CAPABILITIES_PROBLEM_REGISTRY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PROBLEM_REGISTRY "umicom.developer.diagnostics.problem_registry"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_problem_registry(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PROBLEM_REGISTRY_H */
