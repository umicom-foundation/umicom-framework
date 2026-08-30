/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/test_sharding.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Test Sharding developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TEST_SHARDING_H
#define UMICOM_DEVELOPER_CAPABILITIES_TEST_SHARDING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TEST_SHARDING "umicom.developer.testing.test_sharding"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_sharding(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TEST_SHARDING_H */
