/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/dependency_invalidation.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Dependency Invalidation developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_DEPENDENCY_INVALIDATION_H
#define UMICOM_DEVELOPER_CAPABILITIES_DEPENDENCY_INVALIDATION_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_DEPENDENCY_INVALIDATION "umicom.developer.performance.dependency_invalidation"

/**
 * Provide the developer capability dependency invalidation operation used by this module
 * and its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_dependency_invalidation(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_DEPENDENCY_INVALIDATION_H */
