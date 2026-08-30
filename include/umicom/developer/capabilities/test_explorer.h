/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/test_explorer.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Test Explorer developer capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_TEST_EXPLORER_H
#define UMICOM_DEVELOPER_CAPABILITIES_TEST_EXPLORER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_TEST_EXPLORER "umicom.developer.experience.test_explorer"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_explorer(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_TEST_EXPLORER_H */
