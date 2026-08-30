/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/test_fixtures.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Test Fixtures developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TEST_FIXTURES_H
#define UMICOM_DEVELOPER_CAPABILITIES_TEST_FIXTURES_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TEST_FIXTURES "umicom.developer.testing.test_fixtures"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_fixtures(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TEST_FIXTURES_H */
