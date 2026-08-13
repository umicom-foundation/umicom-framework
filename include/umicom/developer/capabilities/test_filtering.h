/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/test_filtering.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Test Filtering developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TEST_FILTERING_H
#define UMICOM_DEVELOPER_CAPABILITIES_TEST_FILTERING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TEST_FILTERING "umicom.developer.testing.test_filtering"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_filtering(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TEST_FILTERING_H */
