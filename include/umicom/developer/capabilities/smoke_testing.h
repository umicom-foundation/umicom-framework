/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/smoke_testing.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Smoke Testing developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_SMOKE_TESTING_H
#define UMICOM_DEVELOPER_CAPABILITIES_SMOKE_TESTING_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_SMOKE_TESTING "umicom.developer.delivery.smoke_testing"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_smoke_testing(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_SMOKE_TESTING_H */
