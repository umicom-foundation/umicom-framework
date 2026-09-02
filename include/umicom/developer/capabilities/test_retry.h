/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/test_retry.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Test Retry developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TEST_RETRY_H
#define UMICOM_DEVELOPER_CAPABILITIES_TEST_RETRY_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TEST_RETRY "umicom.developer.testing.test_retry"

/**
 * Provide the developer capability test retry operation used by this module and its client
 * applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_retry(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TEST_RETRY_H */
