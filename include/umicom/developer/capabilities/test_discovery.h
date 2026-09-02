/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/test_discovery.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Test Discovery developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TEST_DISCOVERY_H
#define UMICOM_DEVELOPER_CAPABILITIES_TEST_DISCOVERY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TEST_DISCOVERY "umicom.developer.testing.test_discovery"

/**
 * Provide the developer capability test discovery operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_discovery(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TEST_DISCOVERY_H */
