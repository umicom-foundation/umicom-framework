/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/parallel_testing.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Parallel Testing developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_TESTING_H
#define UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_TESTING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_PARALLEL_TESTING "umicom.developer.testing.parallel_testing"

/**
 * Provide the developer capability parallel testing operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_parallel_testing(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_PARALLEL_TESTING_H */
