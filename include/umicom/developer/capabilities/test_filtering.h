/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/test_filtering.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Test Filtering developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TEST_FILTERING_H
#define UMICOM_DEVELOPER_CAPABILITIES_TEST_FILTERING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TEST_FILTERING "umicom.developer.testing.test_filtering"

/**
 * Provide the developer capability test filtering operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_filtering(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TEST_FILTERING_H */
