/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/test_timeout.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Test Timeout developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TEST_TIMEOUT_H
#define UMICOM_DEVELOPER_CAPABILITIES_TEST_TIMEOUT_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TEST_TIMEOUT "umicom.developer.testing.test_timeout"

/**
 * Provide the developer capability test timeout operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_timeout(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TEST_TIMEOUT_H */
