/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/capabilities/container_runner.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Container Runner developer capability descriptor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_CAPABILITIES_CONTAINER_RUNNER_H
#define UMICOM_DEVELOPER_CAPABILITIES_CONTAINER_RUNNER_H

#include "umicom/developer/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_CAPABILITY_CONTAINER_RUNNER "umicom.developer.integration.container_runner"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_container_runner(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_CAPABILITIES_CONTAINER_RUNNER_H */
